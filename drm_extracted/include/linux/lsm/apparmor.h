/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Linux Security Module interface to other subsystems.
 * AppArmor presents single pointer to an aa_label structure.
 */
#ifndef __LINUX_LSM_APPARMOR_H
#define __LINUX_LSM_APPARMOR_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../mydefs.h"

struct aa_label;

struct lsm_prop_apparmor {
#ifdef CONFIG_SECURITY_APPARMOR
	struct aa_label *label;
#endif
};

#ifdef __cplusplus
}
#endif
#endif /* ! __LINUX_LSM_APPARMOR_H */
