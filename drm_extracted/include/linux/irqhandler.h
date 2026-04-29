/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_IRQHANDLER_H
#define _LINUX_IRQHANDLER_H

#ifdef __cplusplus
extern "C" {
#endif
#include "mydefs.h"

/*
 * Interrupt flow handler typedefs are defined here to avoid circular
 * include dependencies.
 */

struct irq_desc;

typedef	void (*irq_flow_handler_t)(struct irq_desc *desc);

#ifdef __cplusplus
}
#endif
#endif
