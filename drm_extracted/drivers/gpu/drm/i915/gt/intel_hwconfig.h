/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2022 Intel Corporation
 */

#ifndef _INTEL_HWCONFIG_H_
#define _INTEL_HWCONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"
#include "../../../../../include/linux/types.h"

struct intel_gt;

struct intel_hwconfig {
	u32 size;
	void *ptr;
};

int intel_gt_init_hwconfig(struct intel_gt *gt);
void intel_gt_fini_hwconfig(struct intel_gt *gt);

#ifdef __cplusplus
}
#endif
#endif /* _INTEL_HWCONFIG_H_ */
