#!/bin/bash
# Copyright (c) 2022-2023, NVIDIA CORPORATION.

set -euo pipefail

. /opt/conda/etc/profile.d/conda.sh

rapids-logger "Generate Python testing dependencies"
rapids-dependency-file-generator \
  --output conda \
  --file_key test_python \
  --matrix "cuda=${RAPIDS_CUDA_VERSION%.*};arch=$(arch);py=${RAPIDS_PY_VERSION}" | tee env.yaml

rapids-mamba-retry env create --force -f env.yaml -n test

# Temporarily allow unbound variables for conda activation.
set +u
conda activate test
set -u

rapids-logger "Downloading artifacts from previous jobs"
CPP_CHANNEL=$(rapids-download-conda-from-s3 cpp)
PYTHON_CHANNEL=$(rapids-download-conda-from-s3 python)

RAPIDS_TESTS_DIR=${RAPIDS_TESTS_DIR:-"${PWD}/test-results"}
RAPIDS_COVERAGE_DIR=${RAPIDS_COVERAGE_DIR:-"${PWD}/coverage-results"}
mkdir -p "${RAPIDS_TESTS_DIR}" "${RAPIDS_COVERAGE_DIR}"

rapids-print-env

rapids-mamba-retry install \
  --channel "${CPP_CHANNEL}" \
  --channel "${PYTHON_CHANNEL}" \
  libcugraph \
  pylibcugraph \
  cugraph \
  cugraph-dgl \

rapids-mamba-retry install \
    --force-reinstall \
    --channel pytorch \
    --channel pytorch-nightly \
    --channel dglteam/label/cu117 \
    'pytorch::pytorch>=1.13.1' \
    'pytorch-cuda>=11.7' \
    'dgl'

rapids-logger "Check GPU usage"
nvidia-smi



EXITCODE=0
trap "EXITCODE=1" ERR
set +e

rapids-logger "pytest cugraph_dgl (single GPU)"
pushd python/cugraph-dgl/tests
pytest \
  --cache-clear \
  --ignore=tests/int \
  --ignore=tests/mg \
  --junitxml="${RAPIDS_TESTS_DIR}/junit-cugraph-dgl.xml" \
  --cov-config=../../.coveragerc \
  --cov=cugraph_dgl \
  --cov-report=xml:"${RAPIDS_COVERAGE_DIR}/cugraph-dgl-coverage.xml" \
  --cov-report=term \
  .
popd


rapids-logger "Test script exiting with value: $EXITCODE"
exit ${EXITCODE}
