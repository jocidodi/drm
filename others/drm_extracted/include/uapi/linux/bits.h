/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
/* bits.h: Macros for dealing with bitmasks.  */

#ifndef _UAPI_LINUX_BITS_H
#define _UAPI_LINUX_BITS_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../linux/mydefs.h"

#define __GENMASK(h, l) (((~_UL(0)) << (l)) & (~_UL(0) >> (__BITS_PER_LONG - 1 - (h))))

#define __GENMASK_ULL(h, l) (((~_ULL(0)) << (l)) & (~_ULL(0) >> (__BITS_PER_LONG_LONG - 1 - (h))))

#define __GENMASK_U128(h, l) \
	((_BIT128((h)) << 1) - (_BIT128(l)))

#ifdef __cplusplus
}
#endif
#endif /* _UAPI_LINUX_BITS_H */
