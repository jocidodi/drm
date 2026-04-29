/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_SECTIONS_H
#define _ASM_X86_SECTIONS_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"
#include "../../../../include/asm-generic/sections.h"
#include "extable.h"

extern char __relocate_kernel_start[], __relocate_kernel_end[];
extern char __brk_base[], __brk_limit[];
extern char __end_rodata_aligned[];

#if defined(CONFIG_X86_64)
extern char __end_rodata_hpage_align[];
#endif

extern char __end_of_kernel_reserve[];

extern unsigned long _brk_start, _brk_end;

#ifdef __cplusplus
}
#endif
#endif	/* _ASM_X86_SECTIONS_H */
