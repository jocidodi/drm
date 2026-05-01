/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2019 Intel Corporation
 */

#ifndef INTEL_GT_ENGINES_DEBUGFS_H
#define INTEL_GT_ENGINES_DEBUGFS_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

struct intel_gt;
struct dentry;

void intel_gt_engines_debugfs_register(struct intel_gt *gt, struct dentry *root);

#ifdef __cplusplus
}
#endif
#endif /* INTEL_GT_ENGINES_DEBUGFS_H */
