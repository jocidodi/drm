/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright © 2018 Intel Corporation
 */

#ifndef I915_USER_EXTENSIONS_H
#define I915_USER_EXTENSIONS_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"

struct i915_user_extension;

typedef int (*i915_user_extension_fn)(struct i915_user_extension __user *ext,
				      void *data);

int i915_user_extensions(struct i915_user_extension __user *ext,
			 const i915_user_extension_fn *tbl,
			 unsigned int count,
			 void *data);

#ifdef __cplusplus
}
#endif
#endif /* I915_USER_EXTENSIONS_H */
