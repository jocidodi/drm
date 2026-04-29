/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0 */
/******************************************************************************
 *
 * Module Name: aczephyr.h - OS specific defines, etc.
 *
 * Copyright (C) 2000 - 2025, Intel Corp.
 *
 *****************************************************************************/

#ifndef __ACZEPHYR_H__
#define __ACZEPHYR_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../linux/mydefs.h"

#define ACPI_MACHINE_WIDTH      64

#define ACPI_NO_ERROR_MESSAGES
#undef ACPI_DEBUG_OUTPUT
#define ACPI_USE_SYSTEM_CLIBRARY
#undef ACPI_DBG_TRACK_ALLOCATIONS
#define ACPI_SINGLE_THREADED
#define ACPI_USE_NATIVE_RSDP_POINTER
#include <stdio.h>
#include <stdlib.h>
#include "../../asm-generic/string.h"
#include <ctype.h>
/******************************************************************************
 *
 * FUNCTION:    acpi_enable_dbg_print
 *
 * PARAMETERS:  Enable, 	            - Enable/Disable debug print
 *
 * RETURN:      None
 *
 * DESCRIPTION: Enable/disable debug print
 *
 *****************************************************************************/

void acpi_enable_dbg_print(bool enable);
#ifdef __cplusplus
}
#endif
#endif
