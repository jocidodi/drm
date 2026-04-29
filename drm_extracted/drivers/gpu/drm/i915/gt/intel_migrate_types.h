/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2020 Intel Corporation
 */

#ifndef __INTEL_MIGRATE_TYPES__
#define __INTEL_MIGRATE_TYPES__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

struct intel_context;

struct intel_migrate {
	struct intel_context *context;
};

#ifdef __cplusplus
}
#endif
#endif /* __INTEL_MIGRATE_TYPES__ */
