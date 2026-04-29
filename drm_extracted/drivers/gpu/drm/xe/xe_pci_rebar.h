/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2025 Intel Corporation
 */

#ifndef _XE_PCI_REBAR_H_
#define _XE_PCI_REBAR_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"

struct xe_device;

void xe_pci_rebar_resize(struct xe_device *xe);

#ifdef __cplusplus
}
#endif
#endif
