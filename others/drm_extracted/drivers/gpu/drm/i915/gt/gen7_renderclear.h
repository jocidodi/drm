/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2019 Intel Corporation
 */

#ifndef __GEN7_RENDERCLEAR_H__
#define __GEN7_RENDERCLEAR_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

struct intel_engine_cs;
struct i915_vma;

int gen7_setup_clear_gpr_bb(struct intel_engine_cs * const engine,
			    struct i915_vma * const vma);

#ifdef __cplusplus
}
#endif
#endif /* __GEN7_RENDERCLEAR_H__ */
