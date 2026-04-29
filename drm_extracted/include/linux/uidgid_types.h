/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_UIDGID_TYPES_H
#define _LINUX_UIDGID_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif
#include "mydefs.h"
#include "types.h"

typedef struct {
	uid_t val;
} kuid_t;

typedef struct {
	gid_t val;
} kgid_t;

#ifdef __cplusplus
}
#endif
#endif /* _LINUX_UIDGID_TYPES_H */
