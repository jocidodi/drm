/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __VDSO_JIFFIES_H
#define __VDSO_JIFFIES_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../linux/mydefs.h"
#include "../../arch/x86/include/generated/uapi/asm/param.h"			/* for HZ */
#include "time64.h"

/* TICK_NSEC is the time between ticks in nsec */
#define TICK_NSEC ((NSEC_PER_SEC+HZ/2)/HZ)

#ifdef __cplusplus
}
#endif
#endif /* __VDSO_JIFFIES_H */
