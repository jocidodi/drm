/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_TIMERQUEUE_TYPES_H
#define _LINUX_TIMERQUEUE_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif
#include "mydefs.h"
#include "rbtree_types.h"
#include "types.h"

struct timerqueue_node {
	struct rb_node		node;
	ktime_t			expires;
};

struct timerqueue_head {
	struct rb_root_cached	rb_root;
};

struct timerqueue_linked_node {
	struct rb_node_linked		node;
	ktime_t				expires;
};

struct timerqueue_linked_head {
	struct rb_root_linked		rb_root;
};

#ifdef __cplusplus
}
#endif
#endif /* _LINUX_TIMERQUEUE_TYPES_H */
