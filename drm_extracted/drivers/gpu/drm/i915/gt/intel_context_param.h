/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2019 Intel Corporation
 */

#ifndef INTEL_CONTEXT_PARAM_H
#define INTEL_CONTEXT_PARAM_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"
#include "../../../../../include/linux/types.h"

#include "intel_context.h"

static inline void
intel_context_set_watchdog_us(struct intel_context *ce, u64 timeout_us)
{
	ce->watchdog.timeout_us = timeout_us;
}

#ifdef __cplusplus
}
#endif
#endif /* INTEL_CONTEXT_PARAM_H */
