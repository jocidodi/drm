/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_INAT_TYPES_H
#define _ASM_X86_INAT_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"
/*
 * x86 instruction attributes
 *
 * Written by Masami Hiramatsu <mhiramat@redhat.com>
 */

/* Instruction attributes */
typedef unsigned int insn_attr_t;
typedef unsigned char insn_byte_t;
typedef signed int insn_value_t;

#ifdef __cplusplus
}
#endif
#endif
