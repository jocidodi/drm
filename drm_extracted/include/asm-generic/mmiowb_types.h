/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_GENERIC_MMIOWB_TYPES_H
#define __ASM_GENERIC_MMIOWB_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../linux/mydefs.h"
#include "../linux/types.h"

struct mmiowb_state {
	u16	nesting_count;
	u16	mmiowb_pending;
};

#ifdef __cplusplus
}
#endif
#endif	/* __ASM_GENERIC_MMIOWB_TYPES_H */
