/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright © 2018 Intel Corporation
 */

#ifndef __I915_SELFTESTS_IGT_RESET_H__
#define __I915_SELFTESTS_IGT_RESET_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"
#include "../../../../../include/linux/types.h"

struct intel_gt;

void igt_global_reset_lock(struct intel_gt *gt);
void igt_global_reset_unlock(struct intel_gt *gt);
bool igt_force_reset(struct intel_gt *gt);

#ifdef __cplusplus
}
#endif
#endif
