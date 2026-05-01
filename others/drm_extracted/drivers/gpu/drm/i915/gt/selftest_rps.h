/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2020 Intel Corporation
 */

#ifndef SELFTEST_RPS_H
#define SELFTEST_RPS_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

int live_rps_control(void *arg);
int live_rps_clock_interval(void *arg);
int live_rps_frequency_cs(void *arg);
int live_rps_frequency_srm(void *arg);
int live_rps_power(void *arg);
int live_rps_interrupt(void *arg);
int live_rps_dynamic(void *arg);

#ifdef __cplusplus
}
#endif
#endif /* SELFTEST_RPS_H */
