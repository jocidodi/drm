/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_RESOURCE_H
#define _LINUX_RESOURCE_H

#ifdef __cplusplus
extern "C" {
#endif
#include "mydefs.h"
#include "../uapi/linux/resource.h"


struct task_struct;

void getrusage(struct task_struct *p, int who, struct rusage *ru);

#ifdef __cplusplus
}
#endif
#endif
