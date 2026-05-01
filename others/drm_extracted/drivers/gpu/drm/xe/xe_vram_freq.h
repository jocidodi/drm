/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2023 Intel Corporation
 */

#ifndef _XE_VRAM_FREQ_H_
#define _XE_VRAM_FREQ_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"

struct xe_tile;

int xe_vram_freq_sysfs_init(struct xe_tile *tile);

#ifdef __cplusplus
}
#endif
#endif /* _XE_VRAM_FREQ_H_ */
