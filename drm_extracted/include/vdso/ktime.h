/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __VDSO_KTIME_H
#define __VDSO_KTIME_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../linux/mydefs.h"
#include "jiffies.h"

/*
 * The resolution of the clocks. The resolution value is returned in
 * the clock_getres() system call to give application programmers an
 * idea of the (in)accuracy of timers. Timer values are rounded up to
 * this resolution values.
 */
#define LOW_RES_NSEC		TICK_NSEC
#define KTIME_LOW_RES		(LOW_RES_NSEC)

#ifdef __cplusplus
}
#endif
#endif /* __VDSO_KTIME_H */
