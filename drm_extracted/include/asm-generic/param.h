/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_GENERIC_PARAM_H
#define __ASM_GENERIC_PARAM_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../linux/mydefs.h"
#include "../uapi/asm-generic/param.h"

# undef HZ
# define HZ		CONFIG_HZ	/* Internal kernel timer frequency */
# define USER_HZ	__USER_HZ	/* some user interfaces are */
# define CLOCKS_PER_SEC	(USER_HZ)       /* in "ticks" like times() */
#ifdef __cplusplus
}
#endif
#endif /* __ASM_GENERIC_PARAM_H */
