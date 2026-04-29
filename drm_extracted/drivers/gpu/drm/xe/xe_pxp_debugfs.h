/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2024 Intel Corporation
 */

#ifndef __XE_PXP_DEBUGFS_H__
#define __XE_PXP_DEBUGFS_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"

struct xe_pxp;

void xe_pxp_debugfs_register(struct xe_pxp *pxp);

#ifdef __cplusplus
}
#endif
#endif /* __XE_PXP_DEBUGFS_H__ */
