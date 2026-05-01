/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2022 Intel Corporation
 */

#ifndef _XE_MOCS_H_
#define _XE_MOCS_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"

struct drm_printer;
struct xe_gt;

void xe_mocs_init_early(struct xe_gt *gt);
void xe_mocs_init(struct xe_gt *gt);
int xe_mocs_dump(struct xe_gt *gt, struct drm_printer *p);

#ifdef __cplusplus
}
#endif
#endif
