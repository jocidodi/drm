/* SPDX-License-Identifier: MIT */
/* Copyright © 2025 Intel Corporation */

#ifndef __I915_GMCH_H__
#define __I915_GMCH_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"

struct drm_i915_private;

int i915_gmch_bridge_setup(struct drm_i915_private *i915);
void i915_gmch_bar_setup(struct drm_i915_private *i915);
void i915_gmch_bar_teardown(struct drm_i915_private *i915);

#ifdef __cplusplus
}
#endif
#endif /* __I915_GMCH_H__ */
