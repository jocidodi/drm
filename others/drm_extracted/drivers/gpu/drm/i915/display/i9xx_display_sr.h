/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2024 Intel Corporation
 */

#ifndef __I9XX_DISPLAY_SR_H__
#define __I9XX_DISPLAY_SR_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

struct intel_display;

void i9xx_display_sr_save(struct intel_display *display);
void i9xx_display_sr_restore(struct intel_display *display);

#ifdef __cplusplus
}
#endif
#endif
