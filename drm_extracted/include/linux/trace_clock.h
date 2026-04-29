/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_TRACE_CLOCK_H
#define _LINUX_TRACE_CLOCK_H

#ifdef __cplusplus
extern "C" {
#endif
#include "mydefs.h"

/*
 * 3 trace clock variants, with differing scalability/precision
 * tradeoffs:
 *
 *  -   local: CPU-local trace clock
 *  -  medium: scalable global clock with some jitter
 *  -  global: globally monotonic, serialized clock
 */
#include "compiler.h"
#include "types.h"
#include "../../arch/x86/include/asm/trace_clock.h"

extern u64 notrace trace_clock_local(void);
extern u64 notrace trace_clock(void);
extern u64 notrace trace_clock_jiffies(void);
extern u64 notrace trace_clock_global(void);
extern u64 notrace trace_clock_counter(void);

#ifdef __cplusplus
}
#endif
#endif /* _LINUX_TRACE_CLOCK_H */
