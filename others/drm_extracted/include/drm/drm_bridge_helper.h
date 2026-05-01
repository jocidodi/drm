// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef __DRM_BRIDGE_HELPER_H_
#define __DRM_BRIDGE_HELPER_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "../linux/mydefs.h"

struct drm_bridge;
struct drm_modeset_acquire_ctx;

int drm_bridge_helper_reset_crtc(struct drm_bridge *bridge,
				 struct drm_modeset_acquire_ctx *ctx);

#ifdef __cplusplus
}
#endif
#endif // __DRM_BRIDGE_HELPER_H_
