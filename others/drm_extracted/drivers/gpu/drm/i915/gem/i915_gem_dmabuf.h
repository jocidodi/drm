/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2022 Intel Corporation
 */

#ifndef __I915_GEM_DMABUF_H__
#define __I915_GEM_DMABUF_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

struct drm_gem_object;
struct drm_device;
struct dma_buf;

struct drm_gem_object *i915_gem_prime_import(struct drm_device *dev,
					     struct dma_buf *dma_buf);

struct dma_buf *i915_gem_prime_export(struct drm_gem_object *gem_obj, int flags);

#ifdef __cplusplus
}
#endif
#endif /* __I915_GEM_DMABUF_H__ */
