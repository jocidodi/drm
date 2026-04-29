/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright 2012 Steffen Trumtrar <s.trumtrar@pengutronix.de>
 *
 * videomode of-helpers
 */

#ifndef __LINUX_OF_VIDEOMODE_H
#define __LINUX_OF_VIDEOMODE_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../linux/mydefs.h"

struct device_node;
struct videomode;

int of_get_videomode(struct device_node *np, struct videomode *vm,
		     int index);

#ifdef __cplusplus
}
#endif
#endif /* __LINUX_OF_VIDEOMODE_H */
