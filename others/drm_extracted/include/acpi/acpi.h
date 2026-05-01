/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0 */
/******************************************************************************
 *
 * Name: acpi.h - Master public include file used to interface to ACPICA
 *
 * Copyright (C) 2000 - 2025, Intel Corp.
 *
 *****************************************************************************/

#ifndef __ACPI_H__
#define __ACPI_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../linux/mydefs.h"

/*
 * Public include files for use by code that will interface to ACPICA.
 *
 * Information includes the ACPICA data types, names, exceptions, and
 * external interface prototypes. Also included are the definitions for
 * all ACPI tables (FADT, MADT, etc.)
 *
 * Note: The order of these include files is important.
 */
#include "platform/acenv.h"	/* Environment-specific items */
#include "acnames.h"		/* Common ACPI names and strings */
#include "actypes.h"		/* ACPICA data types and structures */
#include "acexcep.h"		/* ACPICA exceptions */
#include "actbl.h"		/* ACPI table definitions */
#include "acrestyp.h"		/* Resource Descriptor structs */
#include "platform/acenvex.h"	/* Extra environment-specific items */
#include "acoutput.h"		/* Error output and Debug macros */
#include "acpiosxf.h"		/* OSL interfaces (ACPICA-to-OS) */
#include "acpixf.h"		/* ACPI core subsystem external interfaces */

#ifdef __cplusplus
}
#endif
#endif				/* __ACPI_H__ */
