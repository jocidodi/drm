/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2023 Intel Corporation
 */

#ifndef _XE_GSC_PROXY_H_
#define _XE_GSC_PROXY_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../include/linux/mydefs.h"
#include "../../../../include/linux/types.h"

struct xe_gsc;

int xe_gsc_proxy_init(struct xe_gsc *gsc);
bool xe_gsc_proxy_init_done(struct xe_gsc *gsc);
int xe_gsc_wait_for_proxy_init_done(struct xe_gsc *gsc);
int xe_gsc_proxy_start(struct xe_gsc *gsc);

int xe_gsc_proxy_request_handler(struct xe_gsc *gsc);
void xe_gsc_proxy_irq_handler(struct xe_gsc *gsc, u32 iir);

#ifdef __cplusplus
}
#endif
#endif
