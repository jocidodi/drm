/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2021 Intel Corporation
 */

#ifndef _XE_PCI_H_
#define _XE_PCI_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"

struct pci_dev;

int xe_register_pci_driver(void);
void xe_unregister_pci_driver(void);
struct xe_device *xe_pci_to_pf_device(struct pci_dev *pdev);

#ifdef __cplusplus
}
#endif
#endif
