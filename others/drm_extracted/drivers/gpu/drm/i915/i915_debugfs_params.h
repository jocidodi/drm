/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2019 Intel Corporation
 */

#ifndef __I915_DEBUGFS_PARAMS__
#define __I915_DEBUGFS_PARAMS__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"

struct dentry;
struct drm_i915_private;

struct dentry *i915_debugfs_params(struct drm_i915_private *i915);

#ifdef __cplusplus
}
#endif
#endif /* __I915_DEBUGFS_PARAMS__ */
