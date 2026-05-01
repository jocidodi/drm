/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_IRQ_WORK_H
#define _ASM_IRQ_WORK_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"
#include "cpufeature.h"

#ifdef CONFIG_X86_LOCAL_APIC
static inline bool arch_irq_work_has_interrupt(void)
{
	return boot_cpu_has(X86_FEATURE_APIC);
}
#else
static inline bool arch_irq_work_has_interrupt(void)
{
	return false;
}
#endif

#ifdef __cplusplus
}
#endif
#endif /* _ASM_IRQ_WORK_H */
