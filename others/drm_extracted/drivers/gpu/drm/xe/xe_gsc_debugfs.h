/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2024 Intel Corporation
 */

#ifndef _XE_GSC_DEBUGFS_H_
#define _XE_GSC_DEBUGFS_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"

struct dentry;
struct xe_gsc;

void xe_gsc_debugfs_register(struct xe_gsc *gsc, struct dentry *parent);

#ifdef __cplusplus
}
#endif
#endif
