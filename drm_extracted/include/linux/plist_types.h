/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LINUX_PLIST_TYPES_H
#define _LINUX_PLIST_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif
#include "mydefs.h"
#include "types.h"

struct plist_head {
	struct list_head node_list;
};

struct plist_node {
	int			prio;
	struct list_head	prio_list;
	struct list_head	node_list;
};

#ifdef __cplusplus
}
#endif
#endif /* _LINUX_PLIST_TYPES_H */
