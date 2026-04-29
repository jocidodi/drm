/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __VDSO_CACHE_H
#define __VDSO_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../linux/mydefs.h"
#include "../../arch/x86/include/asm/cache.h"

#ifndef SMP_CACHE_BYTES
#define SMP_CACHE_BYTES L1_CACHE_BYTES
#endif

#ifndef ____cacheline_aligned
#define ____cacheline_aligned __attribute__((__aligned__(SMP_CACHE_BYTES)))
#endif

#ifdef __cplusplus
}
#endif
#endif	/* __VDSO_ALIGN_H */
