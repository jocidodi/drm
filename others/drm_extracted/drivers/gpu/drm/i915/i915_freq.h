/* SPDX-License-Identifier: MIT */
/* Copyright © 2025 Intel Corporation */

#ifndef __I915_FREQ_H__
#define __I915_FREQ_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"

struct drm_i915_private;

unsigned int i9xx_fsb_freq(struct drm_i915_private *i915);
unsigned int ilk_fsb_freq(struct drm_i915_private *i915);
unsigned int ilk_mem_freq(struct drm_i915_private *i915);

#ifdef __cplusplus
}
#endif
#endif /* __I915_FREQ_H__ */
