/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2025 Intel Corporation
 */

#ifndef __INTEL_COLOR_PIPELINE_H__
#define __INTEL_COLOR_PIPELINE_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

struct drm_plane;
enum pipe;

int intel_color_pipeline_plane_init(struct drm_plane *plane, enum pipe pipe);

#ifdef __cplusplus
}
#endif
#endif /* __INTEL_COLOR_PIPELINE_H__ */
