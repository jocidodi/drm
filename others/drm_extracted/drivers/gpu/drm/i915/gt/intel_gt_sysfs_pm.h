/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2022 Intel Corporation
 */

#ifndef __SYSFS_GT_PM_H__
#define __SYSFS_GT_PM_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"
#include "../../../../../include/linux/kobject.h"

#include "intel_gt_types.h"

void intel_gt_sysfs_pm_init(struct intel_gt *gt, struct kobject *kobj);

#ifdef __cplusplus
}
#endif
#endif /* SYSFS_RC6_H */
