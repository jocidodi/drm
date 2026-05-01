/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_SET_MEMORY_H
#define __ASM_SET_MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../linux/mydefs.h"

/*
 * Functions to change memory attributes.
 */
int set_memory_ro(unsigned long addr, int numpages);
int set_memory_rw(unsigned long addr, int numpages);
int set_memory_x(unsigned long addr, int numpages);
int set_memory_nx(unsigned long addr, int numpages);

#ifdef __cplusplus
}
#endif
#endif
