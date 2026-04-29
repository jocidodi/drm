/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2023-2024 Intel Corporation
 */

#ifndef _XE_GT_SRIOV_PF_DEBUGFS_H_
#define _XE_GT_SRIOV_PF_DEBUGFS_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"

struct xe_gt;
struct dentry;

#ifdef CONFIG_PCI_IOV
void xe_gt_sriov_pf_debugfs_register(struct xe_gt *gt, struct dentry *root);
void xe_gt_sriov_pf_debugfs_populate(struct xe_gt *gt, struct dentry *parent, unsigned int vfid);
#else
static inline void xe_gt_sriov_pf_debugfs_register(struct xe_gt *gt, struct dentry *root) { }
#endif

#ifdef __cplusplus
}
#endif
#endif
