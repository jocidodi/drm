/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef __LINUX_KMOD_H__
#define __LINUX_KMOD_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "mydefs.h"

/*
 *	include/linux/kmod.h
 */
#include "umh.h"
#include "gfp.h"
#include "stddef.h"
#include "errno.h"
#include "compiler.h"
#include "workqueue.h"
#include "sysctl.h"

#ifdef CONFIG_MODULES
/* modprobe exit status on success, -ve on error.  Return value
 * usually useless though. */
extern __printf(2, 3)
int __request_module(bool wait, const char *name, ...);
#define request_module(mod...) __request_module(true, mod)
#define request_module_nowait(mod...) __request_module(false, mod)
#define try_then_request_module(x, mod...) \
	((x) ?: (__request_module(true, mod), (x)))
#else
static inline int request_module(const char *name, ...) { return -ENOSYS; }
static inline int request_module_nowait(const char *name, ...) { return -ENOSYS; }
#define try_then_request_module(x, mod...) (x)
#endif

#ifdef __cplusplus
}
#endif
#endif /* __LINUX_KMOD_H__ */
