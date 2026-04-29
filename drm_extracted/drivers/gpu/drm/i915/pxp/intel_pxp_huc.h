/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021-2022, Intel Corporation. All rights reserved.
 */

#ifndef __INTEL_PXP_HUC_H__
#define __INTEL_PXP_HUC_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

struct intel_pxp;

int intel_pxp_huc_load_and_auth(struct intel_pxp *pxp);

#ifdef __cplusplus
}
#endif
#endif /* __INTEL_PXP_HUC_H__ */
