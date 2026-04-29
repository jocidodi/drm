/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2019 Intel Corporation
 */

#ifndef __INTEL_DVO_H__
#define __INTEL_DVO_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

struct intel_display;

#ifdef I915
void intel_dvo_init(struct intel_display *display);
#else
static inline void intel_dvo_init(struct intel_display *display)
{
}
#endif

#ifdef __cplusplus
}
#endif
#endif /* __INTEL_DVO_H__ */
