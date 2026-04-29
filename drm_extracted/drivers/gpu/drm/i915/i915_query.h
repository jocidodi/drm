/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright © 2018 Intel Corporation
 */

#ifndef _I915_QUERY_H_
#define _I915_QUERY_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"

struct drm_device;
struct drm_file;

int i915_query_ioctl(struct drm_device *dev, void *data, struct drm_file *file);

#ifdef __cplusplus
}
#endif
#endif
