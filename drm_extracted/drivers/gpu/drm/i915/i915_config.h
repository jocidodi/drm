/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2023 Intel Corporation
 */

#ifndef __I915_CONFIG_H__
#define __I915_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"
#include "../../../../include/linux/types.h"
#include "../../../../include/linux/limits.h"

unsigned long i915_fence_context_timeout(u64 context);

static inline unsigned long i915_fence_timeout(void)
{
	return i915_fence_context_timeout(U64_MAX);
}

#ifdef __cplusplus
}
#endif
#endif /* __I915_CONFIG_H__ */
