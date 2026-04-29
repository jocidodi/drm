/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_SEM_H
#define _LINUX_SEM_H

#ifdef __cplusplus
extern "C" {
#endif
#include "mydefs.h"
#include "../uapi/linux/sem.h"
#include "sem_types.h"

struct task_struct;

#ifdef CONFIG_SYSVIPC

extern int copy_semundo(u64 clone_flags, struct task_struct *tsk);
extern void exit_sem(struct task_struct *tsk);

#else

static inline int copy_semundo(u64 clone_flags, struct task_struct *tsk)
{
	return 0;
}

static inline void exit_sem(struct task_struct *tsk)
{
	return;
}
#endif

#ifdef __cplusplus
}
#endif
#endif /* _LINUX_SEM_H */
