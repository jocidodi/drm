/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2022 Intel Corporation
 */

#ifndef __INTEL_HTI_H__
#define __INTEL_HTI_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"
#include "../../../../../include/linux/types.h"

struct intel_display;
enum phy;

void intel_hti_init(struct intel_display *display);
bool intel_hti_uses_phy(struct intel_display *display, enum phy phy);
u32 intel_hti_dpll_mask(struct intel_display *display);

#ifdef __cplusplus
}
#endif
#endif /* __INTEL_HTI_H__ */
