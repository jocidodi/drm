/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2023 Intel Corporation
 */

#ifndef __INTEL_LOAD_DETECT_H__
#define __INTEL_LOAD_DETECT_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

struct drm_atomic_state;
struct drm_connector;
struct drm_modeset_acquire_ctx;

struct drm_atomic_state *
intel_load_detect_get_pipe(struct drm_connector *connector,
			   struct drm_modeset_acquire_ctx *ctx);
void intel_load_detect_release_pipe(struct drm_connector *connector,
				    struct drm_atomic_state *old,
				    struct drm_modeset_acquire_ctx *ctx);

#ifdef __cplusplus
}
#endif
#endif /* __INTEL_LOAD_DETECT_H__ */
