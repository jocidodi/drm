/* SPDX-License-Identifier: MIT */
/* Copyright © 2024 Intel Corporation */

/*
 * This header is for transitional struct intel_display conversion helpers only.
 */

#ifndef __INTEL_DISPLAY_CONVERSION__
#define __INTEL_DISPLAY_CONVERSION__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

struct drm_device;
struct intel_display;

struct intel_display *__drm_to_display(struct drm_device *drm);

#ifdef __cplusplus
}
#endif
#endif /* __INTEL_DISPLAY_CONVERSION__ */
