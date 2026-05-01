/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2023 Intel Corporation
 */

#ifndef _INTEL_GSC_PROXY_H_
#define _INTEL_GSC_PROXY_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../../include/linux/mydefs.h"
#include "../../../../../../include/linux/types.h"

struct intel_gsc_uc;

int intel_gsc_proxy_init(struct intel_gsc_uc *gsc);
void intel_gsc_proxy_fini(struct intel_gsc_uc *gsc);
int intel_gsc_proxy_request_handler(struct intel_gsc_uc *gsc);
void intel_gsc_proxy_irq_handler(struct intel_gsc_uc *gsc, u32 iir);

#ifdef __cplusplus
}
#endif
#endif
