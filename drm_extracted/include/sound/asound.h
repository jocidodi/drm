/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Advanced Linux Sound Architecture - ALSA - Driver
 *  Copyright (c) 1994-2003 by Jaroslav Kysela <perex@perex.cz>,
 *                             Abramo Bagnara <abramo@alsa-project.org>
 */
#ifndef __SOUND_ASOUND_H
#define __SOUND_ASOUND_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../linux/mydefs.h"
#include "../uapi/linux/ioctl.h"
#include "../linux/time.h"
#include "../../arch/x86/include/uapi/asm/byteorder.h"

#ifdef  __LITTLE_ENDIAN
#define SNDRV_LITTLE_ENDIAN
#else
#ifdef __BIG_ENDIAN
#define SNDRV_BIG_ENDIAN
#else
#error "Unsupported endian..."
#endif
#endif
#include "../uapi/sound/asound.h"
#ifdef __cplusplus
}
#endif
#endif /* __SOUND_ASOUND_H */
