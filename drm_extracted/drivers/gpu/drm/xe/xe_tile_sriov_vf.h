/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2025 Intel Corporation
 */

#ifndef _XE_TILE_SRIOV_VF_H_
#define _XE_TILE_SRIOV_VF_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"
#include "../../../../include/linux/types.h"

struct xe_tile;

u64 xe_tile_sriov_vf_ggtt(struct xe_tile *tile);
void xe_tile_sriov_vf_ggtt_store(struct xe_tile *tile, u64 ggtt_size);
u64 xe_tile_sriov_vf_ggtt_base(struct xe_tile *tile);
void xe_tile_sriov_vf_ggtt_base_store(struct xe_tile *tile, u64 ggtt_size);
u64 xe_tile_sriov_vf_lmem(struct xe_tile *tile);
void xe_tile_sriov_vf_lmem_store(struct xe_tile *tile, u64 lmem_size);

#ifdef __cplusplus
}
#endif
#endif
