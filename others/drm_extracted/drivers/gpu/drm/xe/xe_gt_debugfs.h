/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2022 Intel Corporation
 */

#ifndef _XE_GT_DEBUGFS_H_
#define _XE_GT_DEBUGFS_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"

struct seq_file;
struct xe_gt;

void xe_gt_debugfs_register(struct xe_gt *gt);
int xe_gt_debugfs_simple_show(struct seq_file *m, void *data);
int xe_gt_debugfs_show_with_rpm(struct seq_file *m, void *data);

#ifdef __cplusplus
}
#endif
#endif
