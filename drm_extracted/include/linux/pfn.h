/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_PFN_H_
#define _LINUX_PFN_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "mydefs.h"

#ifndef __ASSEMBLY__
#include "types.h"
#endif

#define PFN_ALIGN(x)	(((unsigned long)(x) + (PAGE_SIZE - 1)) & PAGE_MASK)
#define PFN_UP(x)	(((x) + PAGE_SIZE-1) >> PAGE_SHIFT)
#define PFN_DOWN(x)	((x) >> PAGE_SHIFT)
#define PFN_PHYS(x)	((phys_addr_t)(x) << PAGE_SHIFT)
#define PHYS_PFN(x)	((unsigned long)((x) >> PAGE_SHIFT))

#ifdef __cplusplus
}
#endif
#endif
