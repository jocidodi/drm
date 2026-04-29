/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2021 Intel Corporation
 */

#ifndef __I915_GETPARAM_H__
#define __I915_GETPARAM_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"

struct drm_device;
struct drm_file;

int i915_getparam_ioctl(struct drm_device *dev, void *data,
			struct drm_file *file_priv);

#ifdef __cplusplus
}
#endif
#endif /* __I915_GETPARAM_H__ */
