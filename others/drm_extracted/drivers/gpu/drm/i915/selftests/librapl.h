/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2020 Intel Corporation
 */

#ifndef SELFTEST_LIBRAPL_H
#define SELFTEST_LIBRAPL_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"
#include "../../../../../include/linux/types.h"

struct drm_i915_private;

bool librapl_supported(const struct drm_i915_private *i915);

u64 librapl_energy_uJ(void);

#ifdef __cplusplus
}
#endif
#endif /* SELFTEST_LIBRAPL_H */
