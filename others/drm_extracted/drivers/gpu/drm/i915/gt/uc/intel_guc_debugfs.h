/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2020 Intel Corporation
 */

#ifndef DEBUGFS_GUC_H
#define DEBUGFS_GUC_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../../include/linux/mydefs.h"

struct intel_guc;
struct dentry;

void intel_guc_debugfs_register(struct intel_guc *guc, struct dentry *root);

#ifdef __cplusplus
}
#endif
#endif /* DEBUGFS_GUC_H */
