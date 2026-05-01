/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_PERSONALITY_H
#define _LINUX_PERSONALITY_H

#ifdef __cplusplus
extern "C" {
#endif
#include "mydefs.h"
#include "../uapi/linux/personality.h"

/*
 * Return the base personality without flags.
 */
#define personality(pers)	(pers & PER_MASK)

/*
 * Change personality of the currently running process.
 */
#define set_personality(pers)	(current->personality = (pers))

#ifdef __cplusplus
}
#endif
#endif /* _LINUX_PERSONALITY_H */
