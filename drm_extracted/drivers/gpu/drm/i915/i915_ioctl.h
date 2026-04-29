/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2022 Intel Corporation
 */

#ifndef __I915_IOCTL_H__
#define __I915_IOCTL_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"

struct drm_device;
struct drm_file;

int i915_reg_read_ioctl(struct drm_device *dev, void *data, struct drm_file *file);

#ifdef __cplusplus
}
#endif
#endif /* __I915_IOCTL_H__ */
