/* SPDX-License-Identifier: MIT */
/* Copyright © 2025 Intel Corporation */

#ifndef __INTEL_DISPLAY_UTILS__
#define __INTEL_DISPLAY_UTILS__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../include/linux/mydefs.h"
#include "../../../../../include/linux/bug.h"
#include "../../../../../include/linux/types.h"

struct intel_display;

#define MISSING_CASE(x) WARN(1, "Missing case (%s == %ld)\n", \
			     __stringify(x), (long)(x))

#define fetch_and_zero(ptr) ({						\
	typeof(*ptr) __T = *(ptr);					\
	*(ptr) = (typeof(*ptr))0;					\
	__T;								\
})

#define KHz(x) (1000 * (x))
#define MHz(x) KHz(1000 * (x))

bool intel_display_run_as_guest(struct intel_display *display);
bool intel_display_vtd_active(struct intel_display *display);

#ifdef __cplusplus
}
#endif
#endif /* __INTEL_DISPLAY_UTILS__ */
