/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_SEM_TYPES_H
#define _LINUX_SEM_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif
#include "mydefs.h"

struct sem_undo_list;

struct sysv_sem {
#ifdef CONFIG_SYSVIPC
	struct sem_undo_list *undo_list;
#endif
};

#ifdef __cplusplus
}
#endif
#endif /* _LINUX_SEM_TYPES_H */
