/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2024 Intel Corporation
 */

#ifndef _INTEL_CPU_INFO_H_
#define _INTEL_CPU_INFO_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"
#include "../../../../include/linux/types.h"

bool intel_match_g8_cpu(void);

#ifdef __cplusplus
}
#endif
#endif /* _INTEL_CPU_INFO_H_ */
