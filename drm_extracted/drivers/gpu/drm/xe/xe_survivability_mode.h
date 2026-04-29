/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2025 Intel Corporation
 */

#ifndef _XE_SURVIVABILITY_MODE_H_
#define _XE_SURVIVABILITY_MODE_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"
#include "../../../../include/linux/types.h"

struct xe_device;

int xe_survivability_mode_boot_enable(struct xe_device *xe);
int xe_survivability_mode_runtime_enable(struct xe_device *xe);
bool xe_survivability_mode_is_boot_enabled(struct xe_device *xe);
bool xe_survivability_mode_is_requested(struct xe_device *xe);

#ifdef __cplusplus
}
#endif
#endif /* _XE_SURVIVABILITY_MODE_H_ */
