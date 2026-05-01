/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_WORKQUEUE_TYPES_H
#define _LINUX_WORKQUEUE_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif
#include "mydefs.h"
#include "atomic.h"
#include "lockdep_types.h"
#include "timer_types.h"
#include "types.h"

struct workqueue_struct;

struct work_struct;
typedef void (*work_func_t)(struct work_struct *work);
void delayed_work_timer_fn(struct timer_list *t);

struct work_struct {
	atomic_long_t data;
	struct list_head entry;
	work_func_t func;
#ifdef CONFIG_LOCKDEP
	struct lockdep_map lockdep_map;
#endif
};

#ifdef __cplusplus
}
#endif
#endif /* _LINUX_WORKQUEUE_TYPES_H */
