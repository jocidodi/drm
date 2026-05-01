/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Include file for the interface to IST BIOS
 * Copyright 2002 Andy Grover <andrew.grover@intel.com>
 */
#ifndef _ASM_X86_IST_H
#define _ASM_X86_IST_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"
#include "../uapi/asm/ist.h"


extern struct ist_info ist_info;

#ifdef __cplusplus
}
#endif
#endif /* _ASM_X86_IST_H */
