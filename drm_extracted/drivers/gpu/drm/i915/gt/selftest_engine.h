/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright © 2019 Intel Corporation
 */

#ifndef SELFTEST_ENGINE_H
#define SELFTEST_ENGINE_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

struct intel_gt;

int live_engine_pm_selftests(struct intel_gt *gt);

#ifdef __cplusplus
}
#endif
#endif
