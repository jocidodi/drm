/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __VDSO_TIME_H
#define __VDSO_TIME_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../linux/mydefs.h"
#include "../uapi/linux/types.h"

struct timens_offset {
	s64	sec;
	u64	nsec;
};

#ifdef __cplusplus
}
#endif
#endif /* __VDSO_TIME_H */
