/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_GENERIC_QRWLOCK_TYPES_H
#define __ASM_GENERIC_QRWLOCK_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../linux/mydefs.h"
#include "../linux/types.h"
#include "../../arch/x86/include/uapi/asm/byteorder.h"
#include "../../arch/x86/include/asm/spinlock_types.h"

/*
 * The queued read/write lock data structure
 */

typedef struct qrwlock {
	union {
		atomic_t cnts;
		struct {
#ifdef __LITTLE_ENDIAN
			u8 wlocked;	/* Locked for write? */
			u8 __lstate[3];
#else
			u8 __lstate[3];
			u8 wlocked;	/* Locked for write? */
#endif
		};
	};
	arch_spinlock_t		wait_lock;
} arch_rwlock_t;

#define	__ARCH_RW_LOCK_UNLOCKED {		\
	{ .cnts = ATOMIC_INIT(0), },		\
	.wait_lock = __ARCH_SPIN_LOCK_UNLOCKED,	\
}

#ifdef __cplusplus
}
#endif
#endif /* __ASM_GENERIC_QRWLOCK_TYPES_H */
