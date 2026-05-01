/* SPDX-License-Identifier: MIT */

/*
 * Copyright © 2020 Intel Corporation
 */

#ifndef INTEL_SSEU_DEBUGFS_H
#define INTEL_SSEU_DEBUGFS_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

struct intel_gt;
struct dentry;
struct seq_file;

int intel_sseu_status(struct seq_file *m, struct intel_gt *gt);
void intel_sseu_debugfs_register(struct intel_gt *gt, struct dentry *root);

#ifdef __cplusplus
}
#endif
#endif /* INTEL_SSEU_DEBUGFS_H */
