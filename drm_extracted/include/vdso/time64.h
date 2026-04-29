/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __VDSO_TIME64_H
#define __VDSO_TIME64_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../linux/mydefs.h"

/* Parameters used to convert the timespec values: */
#define MSEC_PER_SEC	1000L
#define USEC_PER_MSEC	1000L
#define NSEC_PER_USEC	1000L
#define NSEC_PER_MSEC	1000000L
#define USEC_PER_SEC	1000000L
#define NSEC_PER_SEC	1000000000L
#define PSEC_PER_SEC	1000000000000LL
#define FSEC_PER_SEC	1000000000000000LL

#ifdef __cplusplus
}
#endif
#endif /* __VDSO_TIME64_H */
