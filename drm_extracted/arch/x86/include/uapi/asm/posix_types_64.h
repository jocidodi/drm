/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
#ifndef _ASM_X86_POSIX_TYPES_64_H
#define _ASM_X86_POSIX_TYPES_64_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

/*
 * This file is generally used by user-level software, so you need to
 * be a little careful about namespace pollution etc.  Also, we cannot
 * assume GCC is being used.
 */

typedef unsigned short __kernel_old_uid_t;
typedef unsigned short __kernel_old_gid_t;
#define __kernel_old_uid_t __kernel_old_uid_t

typedef unsigned long	__kernel_old_dev_t;
#define __kernel_old_dev_t __kernel_old_dev_t
#include "../../../../../include/uapi/asm-generic/posix_types.h"

#ifdef __cplusplus
}
#endif
#endif /* _ASM_X86_POSIX_TYPES_64_H */
