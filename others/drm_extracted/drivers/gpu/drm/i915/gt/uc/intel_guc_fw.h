/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2017-2019 Intel Corporation
 */

#ifndef _INTEL_GUC_FW_H_
#define _INTEL_GUC_FW_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../../include/linux/mydefs.h"

struct intel_guc;

int intel_guc_fw_upload(struct intel_guc *guc);

#ifdef __cplusplus
}
#endif
#endif
