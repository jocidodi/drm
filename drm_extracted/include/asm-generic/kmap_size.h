/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_GENERIC_KMAP_SIZE_H
#define _ASM_GENERIC_KMAP_SIZE_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../linux/mydefs.h"

/* For debug this provides guard pages between the maps */
#ifdef CONFIG_DEBUG_KMAP_LOCAL
# define KM_MAX_IDX	33
#else
# define KM_MAX_IDX	16
#endif

#ifdef __cplusplus
}
#endif
#endif
