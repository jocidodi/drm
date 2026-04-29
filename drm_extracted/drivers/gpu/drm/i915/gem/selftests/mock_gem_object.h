/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright © 2016 Intel Corporation
 */

#ifndef __MOCK_GEM_OBJECT_H__
#define __MOCK_GEM_OBJECT_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../../include/linux/mydefs.h"
#include "../i915_gem_object_types.h"

struct mock_object {
	struct drm_i915_gem_object base;
};

#ifdef __cplusplus
}
#endif
#endif /* !__MOCK_GEM_OBJECT_H__ */
