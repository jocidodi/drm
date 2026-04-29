/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
#ifndef _UAPI_LINUX_STRING_H_
#define _UAPI_LINUX_STRING_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "../../linux/mydefs.h"

/* We don't want strings.h stuff being used by user stuff by accident */

#ifndef __KERNEL__
#include "../../asm-generic/string.h"
#endif /* __KERNEL__ */
#ifdef __cplusplus
}
#endif
#endif /* _UAPI_LINUX_STRING_H_ */
