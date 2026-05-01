/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_MODULE_H
#define _ASM_X86_MODULE_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"
#include "../../../../include/asm-generic/module.h"
#include "orc_types.h"

struct its_array {
#ifdef CONFIG_MITIGATION_ITS
	void **pages;
	int num;
#endif
};

struct mod_arch_specific {
#ifdef CONFIG_UNWINDER_ORC
	unsigned int num_orcs;
	int *orc_unwind_ip;
	struct orc_entry *orc_unwind;
#endif
	struct its_array its_pages;
};

#ifdef __cplusplus
}
#endif
#endif /* _ASM_X86_MODULE_H */
