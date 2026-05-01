/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2023 Intel Corporation
 */

#ifndef __INTEL_DISPLAY_DEBUGFS_PARAMS__
#define __INTEL_DISPLAY_DEBUGFS_PARAMS__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

struct intel_display;

void intel_display_debugfs_params(struct intel_display *display);

#ifdef __cplusplus
}
#endif
#endif /* __INTEL_DISPLAY_DEBUGFS_PARAMS__ */
