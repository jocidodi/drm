/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0 */
/******************************************************************************
 *
 * Name: acenvex.h - Extra host and compiler configuration
 *
 * Copyright (C) 2000 - 2025, Intel Corp.
 *
 *****************************************************************************/

#ifndef __ACENVEX_H__
#define __ACENVEX_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../linux/mydefs.h"

/*! [Begin] no source code translation */

/******************************************************************************
 *
 * Extra host configuration files. All ACPICA headers are included before
 * including these files.
 *
 *****************************************************************************/

#if defined(_LINUX) || defined(__linux__)
#include "aclinuxex.h"

#elif defined(__DragonFly__)
/*
 * EFI applications can be built with -nostdlib, in this case, it must be
 * included after including all other host environmental definitions, in
 * order to override the definitions.
 */
#elif defined(_AED_EFI) || defined(_GNU_EFI) || defined(_EDK2_EFI)
#endif

#if defined(__GNUC__)
#include "acgccex.h"

#elif defined(_MSC_VER)
#endif

/*! [End] no source code translation !*/

#ifdef __cplusplus
}
#endif
#endif				/* __ACENVEX_H__ */
