/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
#ifndef _ASM_X86_MMAN_H
#define _ASM_X86_MMAN_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

#define MAP_32BIT	0x40		/* only give out 32bit addresses */
#define MAP_ABOVE4G	0x80		/* only map above 4GB */
#include "../../../../../include/uapi/asm-generic/mman.h"

#ifdef __cplusplus
}
#endif
#endif /* _ASM_X86_MMAN_H */
