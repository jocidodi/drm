/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_QSPINLOCK_H
#define _ASM_X86_QSPINLOCK_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"
#include "../../../../include/linux/jump_label.h"
#include "cpufeature.h"
#include "../../../../include/asm-generic/qspinlock_types.h"
#include "paravirt.h"
#include "rmwcc.h"
#ifdef CONFIG_PARAVIRT
#include "paravirt-spinlock.h"
#endif

#define _Q_PENDING_LOOPS	(1 << 9)

#define queued_fetch_set_pending_acquire queued_fetch_set_pending_acquire
static __always_inline u32 queued_fetch_set_pending_acquire(struct qspinlock *lock)
{
	u32 val;

	/*
	 * We can't use GEN_BINARY_RMWcc() inside an if() stmt because asm goto
	 * and CONFIG_PROFILE_ALL_BRANCHES=y results in a label inside a
	 * statement expression, which GCC doesn't like.
	 */
	val = GEN_BINARY_RMWcc(LOCK_PREFIX "btsl", lock->val.counter, c,
			       "I", _Q_PENDING_OFFSET) * _Q_PENDING_VAL;
	val |= atomic_read(&lock->val) & ~_Q_PENDING_MASK;

	return val;
}

#ifndef CONFIG_PARAVIRT
static inline void native_pv_lock_init(void) { }
#endif
#include "../../../../include/asm-generic/qspinlock.h"

#ifdef __cplusplus
}
#endif
#endif /* _ASM_X86_QSPINLOCK_H */
