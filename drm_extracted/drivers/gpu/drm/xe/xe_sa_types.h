/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2022 Intel Corporation
 */
#ifndef _XE_SA_TYPES_H_
#define _XE_SA_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"
#include "../../../../include/drm/drm_suballoc.h"

struct xe_bo;

struct xe_sa_manager {
	struct drm_suballoc_manager base;
	struct xe_bo *bo;
	struct xe_bo *shadow;
	/** @swap_guard: Timeline guard updating @bo and @shadow */
	struct mutex swap_guard;
	void *cpu_ptr;
	bool is_iomem;
};

#ifdef __cplusplus
}
#endif
#endif
