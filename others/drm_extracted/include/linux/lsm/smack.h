/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Linux Security Module interface to other subsystems.
 * Smack presents a pointer into the global Smack label list.
 */
#ifndef __LINUX_LSM_SMACK_H
#define __LINUX_LSM_SMACK_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../mydefs.h"

struct smack_known;

struct lsm_prop_smack {
#ifdef CONFIG_SECURITY_SMACK
	struct smack_known *skp;
#endif
};

#ifdef __cplusplus
}
#endif
#endif /* ! __LINUX_LSM_SMACK_H */
