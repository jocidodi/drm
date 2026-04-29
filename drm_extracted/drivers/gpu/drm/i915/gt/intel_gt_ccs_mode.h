/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2024 Intel Corporation
 */

#ifndef __INTEL_GT_CCS_MODE_H__
#define __INTEL_GT_CCS_MODE_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

struct intel_gt;

unsigned int intel_gt_apply_ccs_mode(struct intel_gt *gt);

#ifdef __cplusplus
}
#endif
#endif /* __INTEL_GT_CCS_MODE_H__ */
