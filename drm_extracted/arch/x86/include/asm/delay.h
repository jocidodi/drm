/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_DELAY_H
#define _ASM_X86_DELAY_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"
#include "../../../../include/asm-generic/delay.h"
#include "../../../../include/linux/init.h"

void __init use_tsc_delay(void);
void __init use_tpause_delay(void);
void use_mwaitx_delay(void);

#ifdef __cplusplus
}
#endif
#endif /* _ASM_X86_DELAY_H */
