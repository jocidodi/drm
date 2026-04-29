/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright © 2019 Intel Corporation
 */

#ifndef _I915_ACTIVE_TYPES_H_
#define _I915_ACTIVE_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"
#include "../../../../include/linux/atomic.h"
#include "../../../../include/linux/dma-fence.h"
#include "../../../../include/linux/llist.h"
#include "../../../../include/linux/mutex.h"
#include "../../../../include/linux/rbtree.h"
#include "../../../../include/linux/rcupdate.h"
#include "../../../../include/linux/workqueue.h"

struct i915_active_fence {
	struct dma_fence __rcu *fence;
	struct dma_fence_cb cb;
};

struct active_node;

struct i915_active {
	atomic_t count;
	struct mutex mutex;

	spinlock_t tree_lock;
	struct active_node *cache;
	struct rb_root tree;

	/* Preallocated "exclusive" node */
	struct i915_active_fence excl;

	unsigned long flags;
#define I915_ACTIVE_RETIRE_SLEEPS BIT(0)

	int (*active)(struct i915_active *ref);
	void (*retire)(struct i915_active *ref);

	struct work_struct work;

	struct llist_head preallocated_barriers;
};

#ifdef __cplusplus
}
#endif
#endif /* _I915_ACTIVE_TYPES_H_ */
