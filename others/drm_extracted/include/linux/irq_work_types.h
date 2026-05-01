/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_IRQ_WORK_TYPES_H
#define _LINUX_IRQ_WORK_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif
#include "mydefs.h"
#include "smp_types.h"
#include "types.h"

struct irq_work {
	struct __call_single_node	node;
	void				(*func)(struct irq_work *);
	struct rcuwait			irqwait;
};

#ifdef __cplusplus
}
#endif
#endif
