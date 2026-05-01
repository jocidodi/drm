/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2019 Intel Corporation
 */

#ifndef SELFTEST_RC6_H
#define SELFTEST_RC6_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

int live_rc6_ctx_wa(void *arg);
int live_rc6_manual(void *arg);

#ifdef __cplusplus
}
#endif
#endif /* SELFTEST_RC6_H */
