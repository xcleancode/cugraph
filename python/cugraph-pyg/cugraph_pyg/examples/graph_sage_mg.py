# Copyright (c) 2023, NVIDIA CORPORATION.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# This is a multi-GPU benchmark that assumes the data has already been
# processed using the BulkSampler.  This workflow WILL ONLY WORK when
# reading already-processed sampling results from disk.

import time
import argparse
import gc

import torch

from torch_geometric.nn import CuGraphSAGEConv

import torch.nn as nn
import torch.nn.functional as F

import torch.distributed as td
import torch.multiprocessing as tmp
from torch.nn.parallel import DistributedDataParallel as ddp

from typing import Union, List


class CuGraphSAGE(nn.Module):
    def __init__(self, in_channels, hidden_channels, out_channels, num_layers):
        super().__init__()

        self.convs = torch.nn.ModuleList()
        self.convs.append(CuGraphSAGEConv(in_channels, hidden_channels))
        for _ in range(num_layers - 1):
            conv = CuGraphSAGEConv(hidden_channels, hidden_channels)
            self.convs.append(conv)

        self.lin = nn.Linear(hidden_channels, out_channels)

    def forward(self, x, edge, size):
        edge_csc = CuGraphSAGEConv.to_csc(edge, (size[0], size[0]))
        for conv in self.convs:
            x = conv(x, edge_csc)[:size[1]]
            x = F.relu(x)
            x = F.dropout(x, p=0.5)

        return self.lin(x)


def init_pytorch_worker(rank, devices, manager_ip, manager_port) -> None:
    import cupy
    import rmm

    device_id = devices[rank]

    rmm.reinitialize(
        devices=[device_id],
        pool_allocator=False,
    )

    torch.cuda.change_current_allocator(rmm.rmm_torch_allocator)
    cupy.cuda.set_allocator(rmm.rmm_cupy_allocator)

    cupy.cuda.Device(device_id).use()
    torch.cuda.set_device(device_id)

    # Pytorch training worker initialization
    dist_init_method = f"tcp://{manager_ip}:{manager_port}"

    torch.distributed.init_process_group(
        backend="nccl",
        init_method=dist_init_method,
        world_size=len(devices),
        rank=rank,
    )

def start_cugraph_dask_client(dask_scheduler_address):
    from distributed import Client
    from cugraph.dask.comms import comms as Comms

    client = Client(scheduler_file='/mnt/cugraph/python/cugraph-service/scripts/dask-scheduler.json')
    Comms.initialize(p2p=True)
    return client

def train(rank, dask_scheduler_address: str, torch_devices: List[int], manager_ip: str, manager_port: int, num_epochs: int) -> None:
    """
    Parameters
    ----------
    device: int
        The CUDA device where the model, graph data, and node labels will be stored.
    features_device: Union[str, int]
        The device (CUDA device or CPU) where features will be stored.
    """

    world_size = len(torch_devices)
    device_id = torch_devices[rank]
    init_pytorch_worker(rank, torch_devices, manager_ip, manager_port)
    td.barrier()

    client = start_cugraph_dask_client(dask_scheduler_address)
    
    from distributed import Event as Dask_Event
    event = Dask_Event("cugraph_store_creation_event")
    
    td.barrier()

    import cugraph
    from cugraph_pyg.data import CuGraphStore
    from cugraph_pyg.loader import CuGraphNeighborLoader

    if rank == 0:
        from ogb.nodeproppred import NodePropPredDataset

        dataset = NodePropPredDataset(name='ogbn-mag')
        data = dataset[0]

        G = data[0]['edge_index_dict']
        N = data[0]['num_nodes_dict']

        fs = cugraph.gnn.FeatureStore(backend='torch')
        
        fs.add_data(
            torch.as_tensor(data[0]['node_feat_dict']['paper'], device='cpu'),
            'paper',
            'x'
        )

        fs.add_data(
            torch.as_tensor(data[1]['paper'].T[0], device=device_id),
            'paper',
            'y'
        )

        num_papers = data[0]['num_nodes_dict']['paper']
        train_perc = 0.2
        train_nodes = torch.randperm(num_papers)
        train_nodes = train_nodes[:int(train_perc * num_papers)]
        train_mask = torch.full((num_papers,), -1, device=device_id)
        train_mask[train_nodes] = 1
        fs.add_data(
            train_mask,
            'paper',
            'train'
        )

        cugraph_store = CuGraphStore(fs, G, N, multi_gpu=True)

        import pickle
        client.publish_dataset(cugraph_store=pickle.dumps(cugraph_store))
        client.publish_dataset(train_nodes=train_nodes)
        event.set()
    else:
        if event.wait(timeout=1000):
            import pickle
            cugraph_store = pickle.loads(client.get_dataset('cugraph_store'))
            train_nodes = client.get_dataset('train_nodes')
            train_nodes = train_nodes[int(rank * len(train_nodes) / world_size) : int((rank + 1) * len(train_nodes) / world_size)]

    model = CuGraphSAGE(in_channels=128, hidden_channels=64, out_channels=349,
                num_layers=3).to(torch.float32).to(device_id)
    model = ddp(model, device_ids=[device_id], output_device=device_id)

    optimizer = torch.optim.Adam(model.parameters(), lr=0.01)

    for epoch in range(num_epochs):
        start_time_train = time.perf_counter_ns()
        model.train()

        cugraph_bulk_loader = CuGraphNeighborLoader(
            cugraph_store,
            train_nodes,
            batch_size=500,
            num_neighbors=[10, 25]
        )

        total_loss = 0
        num_batches = 0

        for epoch in range(num_epochs):
            print(f'rank {rank} starting epoch {epoch}')
            with td.algorithms.join.Join([model]):
                for iter_i, hetero_data in enumerate(cugraph_bulk_loader):
                    num_batches += 1
                    print(f'iteration {iter_i}')
                    if iter_i % 50 == 0:
                        print(f'iteration {iter_i}')

                    # train
                    train_mask = hetero_data.train_dict['paper']
                    y_true = hetero_data.y_dict['paper']

                    y_pred = model(
                        hetero_data.x_dict['paper'].to(device_id).to(torch.float32),
                        hetero_data.edge_index_dict[('paper','cites','paper')].to(device_id),
                        (len(y_true), len(y_true))
                    )

                    y_true = F.one_hot(
                        y_true[train_mask].to(torch.int64),
                        num_classes=349
                    ).to(torch.float32)

                    y_pred = y_pred[train_mask]

                    loss = F.cross_entropy(
                        y_pred,
                        y_true
                    )

                    optimizer.zero_grad()
                    loss.backward()
                    optimizer.step()
                    total_loss += loss.item()

                    del y_true
                    del y_pred
                    del loss
                    del hetero_data
                    gc.collect()

                end_time_train = time.perf_counter_ns()
                print(f'epoch {epoch} time: {(end_time_train - start_time_train) / 1e9:3.4f} s')
                print(f'loss after epoch {epoch}: {total_loss / num_batches}')

    if rank == 0:
        print("DONE", flush=True)
        client.unpublish_dataset("cugraph_store")
        client.unpublish_dataset("train_nodes")
        event.clear()

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--torch_devices",
        type=str,
        default="0,1",
        help='GPU to allocate to pytorch for model, graph data, and node label storage',
        required=False
    )

    parser.add_argument(
        "--dask_devices",
        type=str,
        default="0,1",
        help='Device to allocate to pytorch for feature storage',
        required=False
    )

    parser.add_argument(
        "--num_epochs",
        type=int,
        default=1,
        help='Number of training epochs',
        required=False
    )

    parser.add_argument(
        '--torch_manager_ip',
        type=str,
        default="127.0.0.1",
        help='The torch distributed manager ip address',
        required=False,
    )

    parser.add_argument(
        '--torch_manager_port',
        type=str,
        default="12346",
        help='The torch distributed manager port',
        required=False,
    )

    return parser.parse_args()


def start_local_dask_cluster(dask_worker_devices):
    dask_worker_devices_str = ",".join([str(i) for i in dask_worker_devices])
    from dask_cuda import LocalCUDACluster
    from distributed import Client

    cluster = LocalCUDACluster(
        protocol="tcp",
        #protocol='ucx',
        CUDA_VISIBLE_DEVICES=dask_worker_devices_str,
    )

    client = Client(cluster)
    client.wait_for_workers(n_workers=len(dask_worker_devices))
    print("Dask Cluster Setup Complete")
    del client
    return cluster.scheduler_address


def main():
    args = parse_args()

    torch_devices = [int(d) for d in args.torch_devices.split(',')]
    dask_devices = [int(d) for d in args.dask_devices.split(',')]

    #dask_scheduler_address = start_local_dask_cluster(dask_devices)

    train_args = (
        None,
        torch_devices,
        args.torch_manager_ip,
        args.torch_manager_port,
        args.num_epochs
    )

    tmp.spawn(
        train,
        args=train_args,
        nprocs=len(torch_devices)
    )


if __name__ == '__main__':
    main()
