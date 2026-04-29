/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2022 Intel Corporation
 */

#ifndef _XE_GT_SYSFS_TYPES_H_
#define _XE_GT_SYSFS_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"
#include "../../../../include/linux/kobject.h"

struct xe_gt;

/**
 * struct kobj_gt - A GT's kobject struct that connects the kobject and the GT
 *
 * When dealing with multiple GTs, this struct helps to understand which GT
 * needs to be addressed on a given sysfs call.
 */
struct kobj_gt {
	/** @base: The actual kobject */
	struct kobject base;
	/** @gt: A pointer to the GT itself */
	struct xe_gt *gt;
};

#ifdef __cplusplus
}
#endif
#endif	/* _XE_GT_SYSFS_TYPES_H_ */
