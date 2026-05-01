/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_IPC_H
#define _LINUX_IPC_H

#ifdef __cplusplus
extern "C" {
#endif
#include "mydefs.h"
#include "spinlock_types.h"
#include "uidgid.h"
#include "rhashtable-types.h"
#include "../uapi/linux/ipc.h"
#include "refcount.h"

/* used by in-kernel data structures */
struct kern_ipc_perm {
	spinlock_t	lock;
	bool		deleted;
	int		id;
	key_t		key;
	kuid_t		uid;
	kgid_t		gid;
	kuid_t		cuid;
	kgid_t		cgid;
	umode_t		mode;
	unsigned long	seq;
	void		*security;

	struct rhash_head khtnode;

	struct rcu_head rcu;
	refcount_t refcount;
} ____cacheline_aligned_in_smp __randomize_layout;

#ifdef __cplusplus
}
#endif
#endif /* _LINUX_IPC_H */
