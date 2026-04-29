/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2019 Intel Corporation
 */

#ifndef __INTEL_REGION_LMEM_H
#define __INTEL_REGION_LMEM_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

struct intel_gt;

struct intel_memory_region *intel_gt_setup_lmem(struct intel_gt *gt);

#ifdef __cplusplus
}
#endif
#endif /* !__INTEL_REGION_LMEM_H */
