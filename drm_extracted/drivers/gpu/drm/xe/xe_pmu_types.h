/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2025 Intel Corporation
 */

#ifndef _XE_PMU_TYPES_H_
#define _XE_PMU_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"
#include "../../../../include/linux/perf_event.h"
#include "../../../../include/linux/spinlock_types.h"

#define XE_PMU_MAX_GT 2

/**
 * struct xe_pmu - PMU related data per Xe device
 *
 * Stores per device PMU info that includes event/perf attributes and sampling
 * counters across all GTs for this device.
 */
struct xe_pmu {
	/**
	 * @base: PMU base.
	 */
	struct pmu base;
	/**
	 * @registered: PMU is registered and not in the unregistering process.
	 */
	bool registered;
	/**
	 * @name: Name as registered with perf core.
	 */
	const char *name;
	/**
	 * @supported_events: Bitmap of supported events, indexed by event id
	 */
	u64 supported_events;
};

#ifdef __cplusplus
}
#endif
#endif
