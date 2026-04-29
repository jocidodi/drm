/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2019 Intel Corporation
 */

#ifndef __INTEL_DP_AUX_BACKLIGHT_H__
#define __INTEL_DP_AUX_BACKLIGHT_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

struct intel_connector;

int intel_dp_aux_init_backlight_funcs(struct intel_connector *intel_connector);

#ifdef __cplusplus
}
#endif
#endif /* __INTEL_DP_AUX_BACKLIGHT_H__ */
