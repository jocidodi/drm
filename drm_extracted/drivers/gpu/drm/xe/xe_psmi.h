/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2025 Intel Corporation
 */

#ifndef _XE_PSMI_H_
#define _XE_PSMI_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"

struct xe_device;

int xe_psmi_init(struct xe_device *xe);
void xe_psmi_debugfs_register(struct xe_device *xe);

#ifdef __cplusplus
}
#endif
#endif
