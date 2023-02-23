/*
 * Copyright (c) 2022-2023, NVIDIA CORPORATION.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cugraph/algorithms.hpp>

#include "uniform_neighbor_sampling_impl.hpp"

namespace cugraph {

template std::tuple<rmm::device_uvector<int32_t>,
                    rmm::device_uvector<int32_t>,
                    std::optional<rmm::device_uvector<float>>,
                    std::optional<rmm::device_uvector<int32_t>>,
                    std::optional<rmm::device_uvector<int32_t>>,
                    std::optional<rmm::device_uvector<int32_t>>,
                    std::optional<rmm::device_uvector<int32_t>>,
                    std::optional<rmm::device_uvector<size_t>>>
uniform_neighbor_sample(raft::handle_t const& handle,
                        graph_view_t<int32_t, int32_t, false, true> const& graph_view,
                        std::optional<edge_property_view_t<int32_t, float const*>> edge_weight_view,
                        std::optional<edge_property_view_t<int32_t, int32_t const*>> edge_id_view,
                        std::optional<edge_property_view_t<int32_t, int32_t const*>> edge_type_view,
                        rmm::device_uvector<int32_t>&& starting_vertices,
                        std::optional<rmm::device_uvector<int32_t>>&& starting_labels,
                        std::optional<rmm::device_uvector<size_t>>&& starting_vertex_offsets,
                        std::optional<rmm::device_uvector<int32_t>>&& label_to_output_gpu_mapping,
                        raft::host_span<int32_t const> fan_out,
                        raft::random::RngState& rng_state,
                        bool return_hops,
                        bool with_replacement);

template std::tuple<rmm::device_uvector<int32_t>,
                    rmm::device_uvector<int32_t>,
                    std::optional<rmm::device_uvector<float>>,
                    std::optional<rmm::device_uvector<int64_t>>,
                    std::optional<rmm::device_uvector<int32_t>>,
                    std::optional<rmm::device_uvector<int32_t>>,
                    std::optional<rmm::device_uvector<int32_t>>,
                    std::optional<rmm::device_uvector<size_t>>>
uniform_neighbor_sample(raft::handle_t const& handle,
                        graph_view_t<int32_t, int64_t, false, true> const& graph_view,
                        std::optional<edge_property_view_t<int64_t, float const*>> edge_weight_view,
                        std::optional<edge_property_view_t<int64_t, int64_t const*>> edge_id_view,
                        std::optional<edge_property_view_t<int64_t, int32_t const*>> edge_type_view,
                        rmm::device_uvector<int32_t>&& starting_vertices,
                        std::optional<rmm::device_uvector<int32_t>>&& starting_labels,
                        std::optional<rmm::device_uvector<size_t>>&& starting_vertex_offsets,
                        std::optional<rmm::device_uvector<int32_t>>&& label_to_output_gpu_mapping,
                        raft::host_span<int32_t const> fan_out,
                        raft::random::RngState& rng_state,
                        bool return_hops,
                        bool with_replacement);

template std::tuple<rmm::device_uvector<int64_t>,
                    rmm::device_uvector<int64_t>,
                    std::optional<rmm::device_uvector<float>>,
                    std::optional<rmm::device_uvector<int64_t>>,
                    std::optional<rmm::device_uvector<int32_t>>,
                    std::optional<rmm::device_uvector<int32_t>>,
                    std::optional<rmm::device_uvector<int32_t>>,
                    std::optional<rmm::device_uvector<size_t>>>
uniform_neighbor_sample(raft::handle_t const& handle,
                        graph_view_t<int64_t, int64_t, false, true> const& graph_view,
                        std::optional<edge_property_view_t<int64_t, float const*>> edge_weight_view,
                        std::optional<edge_property_view_t<int64_t, int64_t const*>> edge_id_view,
                        std::optional<edge_property_view_t<int64_t, int32_t const*>> edge_type_view,
                        rmm::device_uvector<int64_t>&& starting_vertices,
                        std::optional<rmm::device_uvector<int32_t>>&& starting_labels,
                        std::optional<rmm::device_uvector<size_t>>&& starting_vertex_offsets,
                        std::optional<rmm::device_uvector<int32_t>>&& label_to_output_gpu_mapping,
                        raft::host_span<int32_t const> fan_out,
                        raft::random::RngState& rng_state,
                        bool return_hops,
                        bool with_replacement);

template std::tuple<rmm::device_uvector<int32_t>,
                    rmm::device_uvector<int32_t>,
                    std::optional<rmm::device_uvector<double>>,
                    std::optional<rmm::device_uvector<int32_t>>,
                    std::optional<rmm::device_uvector<int32_t>>,
                    std::optional<rmm::device_uvector<int32_t>>,
                    std::optional<rmm::device_uvector<int32_t>>,
                    std::optional<rmm::device_uvector<size_t>>>
uniform_neighbor_sample(
  raft::handle_t const& handle,
  graph_view_t<int32_t, int32_t, false, true> const& graph_view,
  std::optional<edge_property_view_t<int32_t, double const*>> edge_weight_view,
  std::optional<edge_property_view_t<int32_t, int32_t const*>> edge_id_view,
  std::optional<edge_property_view_t<int32_t, int32_t const*>> edge_type_view,
  rmm::device_uvector<int32_t>&& starting_vertices,
  std::optional<rmm::device_uvector<int32_t>>&& starting_labels,
  std::optional<rmm::device_uvector<size_t>>&& starting_vertex_offsets,
  std::optional<rmm::device_uvector<int32_t>>&& label_to_output_gpu_mapping,
  raft::host_span<int32_t const> fan_out,
  raft::random::RngState& rng_state,
  bool return_hops,
  bool with_replacement);

template std::tuple<rmm::device_uvector<int32_t>,
                    rmm::device_uvector<int32_t>,
                    std::optional<rmm::device_uvector<double>>,
                    std::optional<rmm::device_uvector<int64_t>>,
                    std::optional<rmm::device_uvector<int32_t>>,
                    std::optional<rmm::device_uvector<int32_t>>,
                    std::optional<rmm::device_uvector<int32_t>>,
                    std::optional<rmm::device_uvector<size_t>>>
uniform_neighbor_sample(
  raft::handle_t const& handle,
  graph_view_t<int32_t, int64_t, false, true> const& graph_view,
  std::optional<edge_property_view_t<int64_t, double const*>> edge_weight_view,
  std::optional<edge_property_view_t<int64_t, int64_t const*>> edge_id_view,
  std::optional<edge_property_view_t<int64_t, int32_t const*>> edge_type_view,
  rmm::device_uvector<int32_t>&& starting_vertices,
  std::optional<rmm::device_uvector<int32_t>>&& starting_labels,
  std::optional<rmm::device_uvector<size_t>>&& starting_vertex_offsets,
  std::optional<rmm::device_uvector<int32_t>>&& label_to_output_gpu_mapping,
  raft::host_span<int32_t const> fan_out,
  raft::random::RngState& rng_state,
  bool return_hops,
  bool with_replacement);

template std::tuple<rmm::device_uvector<int64_t>,
                    rmm::device_uvector<int64_t>,
                    std::optional<rmm::device_uvector<double>>,
                    std::optional<rmm::device_uvector<int64_t>>,
                    std::optional<rmm::device_uvector<int32_t>>,
                    std::optional<rmm::device_uvector<int32_t>>,
                    std::optional<rmm::device_uvector<int32_t>>,
                    std::optional<rmm::device_uvector<size_t>>>
uniform_neighbor_sample(
  raft::handle_t const& handle,
  graph_view_t<int64_t, int64_t, false, true> const& graph_view,
  std::optional<edge_property_view_t<int64_t, double const*>> edge_weight_view,
  std::optional<edge_property_view_t<int64_t, int64_t const*>> edge_id_view,
  std::optional<edge_property_view_t<int64_t, int32_t const*>> edge_type_view,
  rmm::device_uvector<int64_t>&& starting_vertices,
  std::optional<rmm::device_uvector<int32_t>>&& starting_labels,
  std::optional<rmm::device_uvector<size_t>>&& starting_vertex_offsets,
  std::optional<rmm::device_uvector<int32_t>>&& label_to_output_gpu_mapping,
  raft::host_span<int32_t const> fan_out,
  raft::random::RngState& rng_state,
  bool return_hops,
  bool with_replacement);

}  // namespace cugraph
