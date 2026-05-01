/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2023 Intel Corporation
 */

#ifndef __INTEL_HDCP_GSC_MESSAGE_H__
#define __INTEL_HDCP_GSC_MESSAGE_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"

struct intel_display;

int intel_hdcp_gsc_init(struct intel_display *display);
void intel_hdcp_gsc_fini(struct intel_display *display);

#ifdef __cplusplus
}
#endif
#endif /* __INTEL_HDCP_GSC_MESSAGE_H__ */
