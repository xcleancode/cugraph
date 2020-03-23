/*
 * Copyright (c) 2020, NVIDIA CORPORATION.
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

#pragma once

#include "bh_kernels.h"

namespace cugraph {
namespace detail {

template <typename vertex_t, typename edge_t, typename weight_t>
void barnes_hut(const edge_t *csrPtr, const vertex_t *csrInd,
        const weight_t *v, const vertex_t n, float *x_pos,
        float *y_pos, int max_iter, float *x_start, float *y_start,
        bool outbount_attraction_distribution, bool lin_log_mode,
        bool prevent_overlapping, float edge_weight_influence,
        float jitter_tolerance, float barnes_hut_theta, float scaling_ratio,
        bool strong_gravity_mode, float gravity) {
    return;
}

} // namespace detail
} // namespace cugraph
