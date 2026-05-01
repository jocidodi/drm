/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2023 Intel Corporation
 */

#ifndef __INTEL_DPT_COMMON_H__
#define __INTEL_DPT_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

struct intel_crtc;
struct intel_display;

void intel_dpt_configure(struct intel_crtc *crtc);
void intel_dpt_suspend(struct intel_display *display);
void intel_dpt_resume(struct intel_display *display);

#ifdef __cplusplus
}
#endif
#endif /* __INTEL_DPT_COMMON_H__ */
