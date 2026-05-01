/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __VDSO_CONST_H
#define __VDSO_CONST_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../linux/mydefs.h"
#include "../uapi/linux/const.h"

#define UL(x)		(_UL(x))
#define ULL(x)		(_ULL(x))

#ifdef __cplusplus
}
#endif
#endif /* __VDSO_CONST_H */
