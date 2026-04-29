/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2021 Intel Corporation
 */

#ifndef __INTEL_DP_HDCP___
#define __INTEL_DP_HDCP___

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

struct intel_connector;
struct intel_digital_port;

int intel_dp_hdcp_init(struct intel_digital_port *dig_port,
		       struct intel_connector *intel_connector);

#ifdef __cplusplus
}
#endif
#endif /* __INTEL_DP_HDCP___ */
