/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright © 2018 Intel Corporation
 */

#ifndef IGT_FLUSH_TEST_H
#define IGT_FLUSH_TEST_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

struct drm_i915_private;

int igt_flush_test(struct drm_i915_private *i915);

#ifdef __cplusplus
}
#endif
#endif /* IGT_FLUSH_TEST_H */
