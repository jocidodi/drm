// SPDX-License-Identifier: MIT
/*
 * Copyright © 2024 Intel Corporation
 */
#include "../../i915/display/intel_display_core.h"
#include "../../i915/display/intel_tdf.h"
#include "../xe_device.h"

void intel_td_flush(struct intel_display *display)
{
	struct xe_device *xe = to_xe_device(display->drm);

	xe_device_td_flush(xe);
}
