/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2021 Intel Corporation
 */

#ifndef __I915_GEM_CREATE_H__
#define __I915_GEM_CREATE_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

struct drm_file;
struct drm_device;
struct drm_mode_create_dumb;

int i915_gem_dumb_create(struct drm_file *file_priv,
			 struct drm_device *dev,
			 struct drm_mode_create_dumb *args);

#ifdef __cplusplus
}
#endif
#endif /* __I915_GEM_CREATE_H__ */
