/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_CACHEFLUSH_H
#define _ASM_X86_CACHEFLUSH_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"
#include "../../../../include/linux/mm.h"

/* Caches aren't brain-dead on the intel. */
#include "../../../../include/asm-generic/cacheflush.h"
#include "special_insns.h"

void clflush_cache_range(void *addr, unsigned int size);

#ifdef __cplusplus
}
#endif
#endif /* _ASM_X86_CACHEFLUSH_H */
