/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2023 Intel Corporation
 */

#ifndef _XE_TTM_SYS_MGR_H_
#define _XE_TTM_SYS_MGR_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"

struct xe_device;

int xe_ttm_sys_mgr_init(struct xe_device *xe);

#ifdef __cplusplus
}
#endif
#endif
