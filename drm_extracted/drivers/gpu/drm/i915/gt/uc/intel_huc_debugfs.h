/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2020 Intel Corporation
 */

#ifndef DEBUGFS_HUC_H
#define DEBUGFS_HUC_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../../include/linux/mydefs.h"

struct intel_huc;
struct dentry;

void intel_huc_debugfs_register(struct intel_huc *huc, struct dentry *root);

#ifdef __cplusplus
}
#endif
#endif /* DEBUGFS_HUC_H */
