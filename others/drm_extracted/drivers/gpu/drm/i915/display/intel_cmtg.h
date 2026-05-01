/* SPDX-License-Identifier: MIT */
/*
 * Copyright (C) 2025 Intel Corporation
 */

#ifndef __INTEL_CMTG_H__
#define __INTEL_CMTG_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

struct intel_display;

void intel_cmtg_sanitize(struct intel_display *display);

#ifdef __cplusplus
}
#endif
#endif /* __INTEL_CMTG_H__ */
