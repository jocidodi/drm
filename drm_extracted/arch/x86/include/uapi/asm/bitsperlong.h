/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
#ifndef __ASM_X86_BITSPERLONG_H
#define __ASM_X86_BITSPERLONG_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

#if defined(__x86_64__) && !defined(__ILP32__)
# define __BITS_PER_LONG 64
#else
# define __BITS_PER_LONG 32
#endif
#include "../../../../../include/asm-generic/bitsperlong.h"

#ifdef __cplusplus
}
#endif
#endif /* __ASM_X86_BITSPERLONG_H */

