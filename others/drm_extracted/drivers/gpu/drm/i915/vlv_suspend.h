/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2020 Intel Corporation
 */

#ifndef __VLV_SUSPEND_H__
#define __VLV_SUSPEND_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"
#include "../../../../include/linux/types.h"

struct drm_i915_private;

int vlv_suspend_init(struct drm_i915_private *i915);
void vlv_suspend_cleanup(struct drm_i915_private *i915);
int vlv_suspend_complete(struct drm_i915_private *i915);
int vlv_resume_prepare(struct drm_i915_private *i915, bool rpm_resume);

#ifdef __cplusplus
}
#endif
#endif /* __VLV_SUSPEND_H__ */
