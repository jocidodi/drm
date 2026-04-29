/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2023 Intel Corporation
 */

#ifndef DEBUGFS_GSC_UC_H
#define DEBUGFS_GSC_UC_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../../include/linux/mydefs.h"

struct intel_gsc_uc;
struct dentry;

void intel_gsc_uc_debugfs_register(struct intel_gsc_uc *gsc, struct dentry *root);

#ifdef __cplusplus
}
#endif
#endif /* DEBUGFS_GSC_UC_H */
