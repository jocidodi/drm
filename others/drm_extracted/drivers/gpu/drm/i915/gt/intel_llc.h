/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2019 Intel Corporation
 */

#ifndef INTEL_LLC_H
#define INTEL_LLC_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

struct intel_llc;

void intel_llc_enable(struct intel_llc *llc);
void intel_llc_disable(struct intel_llc *llc);

#ifdef __cplusplus
}
#endif
#endif /* INTEL_LLC_H */
