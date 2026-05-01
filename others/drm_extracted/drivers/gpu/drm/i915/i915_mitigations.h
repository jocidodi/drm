/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2021 Intel Corporation
 */

#ifndef __I915_MITIGATIONS_H__
#define __I915_MITIGATIONS_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"
#include "../../../../include/linux/types.h"

bool i915_mitigate_clear_residuals(void);

#ifdef __cplusplus
}
#endif
#endif /* __I915_MITIGATIONS_H__ */
