/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2021 Intel Corporation
 */
#ifndef __INTEL_SA_MEDIA__
#define __INTEL_SA_MEDIA__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"
#include "../../../../../include/linux/types.h"

struct intel_gt;

int intel_sa_mediagt_setup(struct intel_gt *gt, phys_addr_t phys_addr,
			   u32 gsi_offset);

#ifdef __cplusplus
}
#endif
#endif /* __INTEL_SA_MEDIA_H__ */
