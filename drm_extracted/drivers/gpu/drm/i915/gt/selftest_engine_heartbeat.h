/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2020 Intel Corporation
 */

#ifndef SELFTEST_ENGINE_HEARTBEAT_H
#define SELFTEST_ENGINE_HEARTBEAT_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

struct intel_engine_cs;

void st_engine_heartbeat_disable(struct intel_engine_cs *engine);
void st_engine_heartbeat_disable_no_pm(struct intel_engine_cs *engine);
void st_engine_heartbeat_enable(struct intel_engine_cs *engine);
void st_engine_heartbeat_enable_no_pm(struct intel_engine_cs *engine);

#ifdef __cplusplus
}
#endif
#endif /* SELFTEST_ENGINE_HEARTBEAT_H */
