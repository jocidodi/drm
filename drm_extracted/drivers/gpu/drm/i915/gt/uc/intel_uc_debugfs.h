/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2020 Intel Corporation
 */

#ifndef DEBUGFS_UC_H
#define DEBUGFS_UC_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../../include/linux/mydefs.h"

struct intel_uc;
struct dentry;

void intel_uc_debugfs_register(struct intel_uc *uc, struct dentry *gt_root);

#ifdef __cplusplus
}
#endif
#endif /* DEBUGFS_UC_H */
