/* SPDX-License-Identifier: GPL-2.0 */
#ifndef ACPI_BUTTON_H
#define ACPI_BUTTON_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../linux/mydefs.h"

#define ACPI_BUTTON_HID_POWER	"PNP0C0C"
#define ACPI_BUTTON_HID_LID	"PNP0C0D"
#define ACPI_BUTTON_HID_SLEEP	"PNP0C0E"

#if IS_ENABLED(CONFIG_ACPI_BUTTON)
extern int acpi_lid_open(void);
#else
static inline int acpi_lid_open(void)
{
	return 1;
}
#endif /* IS_ENABLED(CONFIG_ACPI_BUTTON) */

#ifdef __cplusplus
}
#endif
#endif /* ACPI_BUTTON_H */
