/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2025 Intel Corporation
 */

#ifndef _XE_SOC_REMAPPER_H_
#define _XE_SOC_REMAPPER_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"

#include "xe_device_types.h"

int xe_soc_remapper_init(struct xe_device *xe);

#ifdef __cplusplus
}
#endif
#endif
