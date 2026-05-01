/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_IOMAP_H
#define _ASM_X86_IOMAP_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"

/*
 * Copyright © 2008 Ingo Molnar
 */
#include "../../../../include/linux/fs.h"
#include "../../../../include/linux/mm.h"
#include "../../../../include/linux/uaccess.h"
#include "../../../../include/linux/highmem.h"
#include "cacheflush.h"
#include "tlbflush.h"

void __iomem *__iomap_local_pfn_prot(unsigned long pfn, pgprot_t prot);

int iomap_create_wc(resource_size_t base, unsigned long size, pgprot_t *prot);

void iomap_free(resource_size_t base, unsigned long size);

#ifdef __cplusplus
}
#endif
#endif /* _ASM_X86_IOMAP_H */
