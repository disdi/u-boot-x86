#define INCLUDE_LPE  1
#define INCLUDE_SCC  1
#define INCLUDE_EHCI 1
#define INCLUDE_XHCI 1
#define INCLUDE_LPSS 1


DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	0x02,		// DSDT revision: ACPI v2.0
	"COREv4",	// OEM id
	"UBOOT",	// OEM table id
	0x20110725	// OEM revision
)
{
	// Some generic macros
	#include "acpi/platform.asl"

	// global NVS and variables
	#include "acpi/globalnvs.asl"

	#include "acpi/cpu.asl"

	Scope (\_SB) {
		Device (PCI0)
		{
			#include "acpi/southcluster.asl"
		}
	}
	/* Chipset specific sleep states */
	#include "acpi/sleepstates.asl"

	#include "acpi/mainboard.asl"
}
