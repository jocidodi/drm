/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2025 Intel Corporation
 */

#ifndef __INTEL_COLOROP_H__
#define __INTEL_COLOROP_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

enum intel_color_block;
struct drm_colorop;
struct intel_colorop;

struct intel_colorop *to_intel_colorop(struct drm_colorop *colorop);
struct intel_colorop *intel_colorop_alloc(void);
struct intel_colorop *intel_colorop_create(enum intel_color_block id);
void intel_colorop_destroy(struct drm_colorop *colorop);

#ifdef __cplusplus
}
#endif
#endif /* __INTEL_COLOROP_H__ */
