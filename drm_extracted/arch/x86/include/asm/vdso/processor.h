/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2020 ARM Ltd.
 */
#ifndef __ASM_VDSO_PROCESSOR_H
#define __ASM_VDSO_PROCESSOR_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

#ifndef __ASSEMBLER__

/* PAUSE is a good thing to insert into busy-wait loops. */
static __always_inline void native_pause(void)
{
	asm volatile("pause" ::: "memory");
}

static __always_inline void cpu_relax(void)
{
	native_pause();
}

notrace long __vdso_getcpu(unsigned *cpu, unsigned *node, void *unused);

#endif /* __ASSEMBLER__ */

#ifdef __cplusplus
}
#endif
#endif /* __ASM_VDSO_PROCESSOR_H */
