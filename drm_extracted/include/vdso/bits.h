/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __VDSO_BITS_H
#define __VDSO_BITS_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../linux/mydefs.h"
#include "const.h"

#define BIT(nr)			(UL(1) << (nr))
#define BIT_ULL(nr)		(ULL(1) << (nr))

#ifdef __cplusplus
}
#endif
#endif	/* __VDSO_BITS_H */
