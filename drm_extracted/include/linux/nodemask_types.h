/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_NODEMASK_TYPES_H
#define __LINUX_NODEMASK_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif
#include "mydefs.h"
#include "bitops.h"

#ifdef CONFIG_NODES_SHIFT
#define NODES_SHIFT     CONFIG_NODES_SHIFT
#else
#define NODES_SHIFT     0
#endif

#define MAX_NUMNODES    (1 << NODES_SHIFT)

#define	NUMA_NO_NODE	(-1)

typedef struct { DECLARE_BITMAP(bits, MAX_NUMNODES); } nodemask_t;

#ifdef __cplusplus
}
#endif
#endif /* __LINUX_NODEMASK_TYPES_H */
