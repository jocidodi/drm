/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2023 Intel Corporation
 */

#ifndef _XE_TILE_SYSFS_TYPES_H_
#define _XE_TILE_SYSFS_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"
#include "../../../../include/linux/kobject.h"

struct xe_tile;

/**
 * struct kobj_tile - A tile's kobject struct that connects the kobject
 * and the TILE
 *
 * When dealing with multiple TILEs, this struct helps to understand which
 * TILE needs to be addressed on a given sysfs call.
 */
struct kobj_tile {
	/** @base: The actual kobject */
	struct kobject base;
	/** @tile: A pointer to the tile itself */
	struct xe_tile *tile;
};

#ifdef __cplusplus
}
#endif
#endif	/* _XE_TILE_SYSFS_TYPES_H_ */
