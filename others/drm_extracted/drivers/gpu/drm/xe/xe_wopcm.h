/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2022 Intel Corporation
 */

#ifndef _XE_WOPCM_H_
#define _XE_WOPCM_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"

#include "xe_wopcm_types.h"

struct xe_device;

int xe_wopcm_init(struct xe_wopcm *wopcm);
u32 xe_wopcm_size(struct xe_device *xe);

#ifdef __cplusplus
}
#endif
#endif
