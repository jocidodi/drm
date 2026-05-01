/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2022 Intel Corporation
 */

#ifndef __INTEL_MODESET_SETUP_H__
#define __INTEL_MODESET_SETUP_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

struct drm_modeset_acquire_ctx;
struct intel_display;

void intel_modeset_setup_hw_state(struct intel_display *display,
				  struct drm_modeset_acquire_ctx *ctx);

#ifdef __cplusplus
}
#endif
#endif /* __INTEL_MODESET_SETUP_H__ */
