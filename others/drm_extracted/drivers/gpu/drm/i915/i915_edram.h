/* SPDX-License-Identifier: MIT */
/* Copyright © 2025 Intel Corporation */

#ifndef __I915_DRAM_H__
#define __I915_DRAM_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"

struct drm_i915_private;

void i915_edram_detect(struct drm_i915_private *i915);

#ifdef __cplusplus
}
#endif
#endif /* __I915_DRAM_H__ */
