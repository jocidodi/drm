/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2023-2024 Intel Corporation
 */

#ifndef _XE_GT_SRIOV_VF_DEBUGFS_H_
#define _XE_GT_SRIOV_VF_DEBUGFS_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"

struct xe_gt;
struct dentry;

void xe_gt_sriov_vf_debugfs_register(struct xe_gt *gt, struct dentry *root);

#ifdef __cplusplus
}
#endif
#endif
