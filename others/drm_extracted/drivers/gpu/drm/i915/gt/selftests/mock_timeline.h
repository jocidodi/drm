/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright © 2017-2018 Intel Corporation
 */

#ifndef __MOCK_TIMELINE__
#define __MOCK_TIMELINE__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../../include/linux/mydefs.h"
#include "../../../../../../include/linux/types.h"

struct intel_timeline;

void mock_timeline_init(struct intel_timeline *timeline, u64 context);
void mock_timeline_fini(struct intel_timeline *timeline);

#ifdef __cplusplus
}
#endif
#endif /* !__MOCK_TIMELINE__ */
