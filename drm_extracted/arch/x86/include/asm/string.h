/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_STRING_H
#define _ASM_X86_STRING_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"

#ifdef CONFIG_X86_32
#include "string_32.h"
#else
#include "string_64.h"
#endif

static __always_inline void *__inline_memcpy(void *to, const void *from, size_t len)
{
	void *ret = to;

	asm volatile("rep movsb"
		     : "+D" (to), "+S" (from), "+c" (len)
		     : : "memory");
	return ret;
}

static __always_inline void *__inline_memset(void *s, int v, size_t n)
{
	void *ret = s;

	asm volatile("rep stosb"
		     : "+D" (s), "+c" (n)
		     : "a" ((uint8_t)v)
		     : "memory");
	return ret;
}

#ifdef __cplusplus
}
#endif
#endif /* _ASM_X86_STRING_H */
