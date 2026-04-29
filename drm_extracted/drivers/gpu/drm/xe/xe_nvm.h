/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2019-2025 Intel Corporation. All rights reserved.
 */

#ifndef __XE_NVM_H__
#define __XE_NVM_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"

struct xe_device;

int xe_nvm_init(struct xe_device *xe);

#ifdef __cplusplus
}
#endif
#endif
