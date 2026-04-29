/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
#ifndef _UAPI_ASM_X86_VSYSCALL_H
#define _UAPI_ASM_X86_VSYSCALL_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

enum vsyscall_num {
	__NR_vgettimeofday,
	__NR_vtime,
	__NR_vgetcpu,
};

#define VSYSCALL_ADDR (-10UL << 20)

#ifdef __cplusplus
}
#endif
#endif /* _UAPI_ASM_X86_VSYSCALL_H */
