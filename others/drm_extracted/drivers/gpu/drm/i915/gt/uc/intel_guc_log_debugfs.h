/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2020 Intel Corporation
 */

#ifndef DEBUGFS_GUC_LOG_H
#define DEBUGFS_GUC_LOG_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../../include/linux/mydefs.h"

struct intel_guc_log;
struct dentry;

void intel_guc_log_debugfs_register(struct intel_guc_log *log,
				    struct dentry *root);

#ifdef __cplusplus
}
#endif
#endif /* DEBUGFS_GUC_LOG_H */
