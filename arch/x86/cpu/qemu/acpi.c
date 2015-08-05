#include <asm/acpi_table.h>
#include <asm/cpu.h>
#include <asm/ioapic.h>
#include <asm/lapic.h>
#include <asm/tables.h>
#include <asm/pci.h>
#include <cpu.h>
#include <dm.h>
#include <dm/uclass-internal.h>
#include <dm/lists.h>

void acpi_create_fadt(struct acpi_fadt * fadt, struct acpi_facs * facs, void *dsdt)
{
	acpi_header_t *header = &(fadt->header);
	u16 pmbase;

	pci_dev_t bdf = PCI_BDF(0, 0x1f, 0);
	pci_read_config_word(bdf, 0x40, &pmbase);

        //wrong value of pmbase by above function. Harcoding it to correct value.
	pmbase = 0x0600;

	memset((void *) fadt, 0, sizeof(struct acpi_fadt));
	memcpy(header->signature, "FACP", 4);
	header->length = sizeof(struct acpi_fadt);
	header->revision = 3;
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);
	header->asl_compiler_revision = 0;

	fadt->firmware_ctrl = (unsigned long) facs;
	fadt->dsdt = (unsigned long) dsdt;
	fadt->model = 0x00;
	fadt->preferred_pm_profile = PM_MOBILE;
	fadt->sci_int = 0x9;
	fadt->smi_cmd = 0;
	fadt->acpi_enable = 0;
	fadt->acpi_disable = 0;
	fadt->s4bios_req = 0x0;
	fadt->pstate_cnt = 0;
	fadt->pm1a_evt_blk = pmbase;
	fadt->pm1b_evt_blk = 0x0;
	fadt->pm1a_cnt_blk = pmbase + 0x4;
	fadt->pm1b_cnt_blk = 0x0;
	fadt->pm2_cnt_blk = pmbase + 0x50;
	fadt->pm_tmr_blk = pmbase + 0x8;
	fadt->gpe0_blk = pmbase + 0x20;
	fadt->gpe1_blk = 0;
	fadt->pm1_evt_len = 4;
	fadt->pm1_cnt_len = 2; /* Upper word is reserved and
				  Linux complains about 32 bit. */
	fadt->pm2_cnt_len = 1;
	fadt->pm_tmr_len = 4;
	fadt->gpe0_blk_len = 16;
	fadt->gpe1_blk_len = 0;
	fadt->gpe1_base = 0;
	fadt->cst_cnt = 0;
	fadt->p_lvl2_lat = 1;
	fadt->p_lvl3_lat = 0x39;
	fadt->flush_size = 0;
	fadt->flush_stride = 0;
	fadt->duty_offset = 1;
	fadt->duty_width = 3;
	fadt->day_alrm = 0xd;
	fadt->mon_alrm = 0x00;
	fadt->century = 0x32;
	fadt->iapc_boot_arch = 0x00;
	fadt->flags = ACPI_FADT_WBINVD | ACPI_FADT_C1_SUPPORTED |
			ACPI_FADT_SLEEP_BUTTON | ACPI_FADT_S4_RTC_WAKE |
			ACPI_FADT_DOCKING_SUPPORTED | ACPI_FADT_RESET_REGISTER |
			ACPI_FADT_PLATFORM_CLOCK;
	fadt->reset_reg.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->reset_reg.bit_width = 8;
	fadt->reset_reg.bit_offset = 0;
	fadt->reset_reg.resv = 0;
	fadt->reset_reg.addrl = 0xcf9;
	fadt->reset_reg.addrh = 0;
	fadt->reset_value = 0x06;
	fadt->x_firmware_ctl_l = 0; /* Set X_FIRMWARE_CTRL only if FACS is */
	fadt->x_firmware_ctl_h = 0; /* above 4GB. If X_FIRMWARE_CTRL is set, */
				    /* then FIRMWARE_CTRL must be zero. */

	fadt->x_dsdt_l = (unsigned long)dsdt;
	fadt->x_dsdt_h = 0;
	fadt->x_pm1a_evt_blk.space_id = 1;
	fadt->x_pm1a_evt_blk.bit_width = 32;
	fadt->x_pm1a_evt_blk.bit_offset = 0;
	fadt->x_pm1a_evt_blk.resv = 0;
	fadt->x_pm1a_evt_blk.addrl = pmbase;
	fadt->x_pm1a_evt_blk.addrh = 0x0;
	fadt->x_pm1b_evt_blk.space_id = 0;
	fadt->x_pm1b_evt_blk.bit_width = 0;
	fadt->x_pm1b_evt_blk.bit_offset = 0;
	fadt->x_pm1b_evt_blk.resv = 0;
	fadt->x_pm1b_evt_blk.addrl = 0x0;
	fadt->x_pm1b_evt_blk.addrh = 0x0;
	fadt->x_pm1a_cnt_blk.space_id = 1;
	fadt->x_pm1a_cnt_blk.bit_width = 16; /* Upper word is reserved and
						Linux complains about 32 bit. */
	fadt->x_pm1a_cnt_blk.bit_offset = 0;
	fadt->x_pm1a_cnt_blk.resv = 0;
	fadt->x_pm1a_cnt_blk.addrl = pmbase + 0x4;
	fadt->x_pm1a_cnt_blk.addrh = 0x0;
	fadt->x_pm1b_cnt_blk.space_id = 0;
	fadt->x_pm1b_cnt_blk.bit_width = 0;
	fadt->x_pm1b_cnt_blk.bit_offset = 0;
	fadt->x_pm1b_cnt_blk.resv = 0;
	fadt->x_pm1b_cnt_blk.addrl = 0x0;
	fadt->x_pm1b_cnt_blk.addrh = 0x0;
	fadt->x_pm2_cnt_blk.space_id = 1;
	fadt->x_pm2_cnt_blk.bit_width = 8;
	fadt->x_pm2_cnt_blk.bit_offset = 0;
	fadt->x_pm2_cnt_blk.resv = 0;
	fadt->x_pm2_cnt_blk.addrl = pmbase + 0x50;
	fadt->x_pm2_cnt_blk.addrh = 0x0;
	fadt->x_pm_tmr_blk.space_id = 1;
	fadt->x_pm_tmr_blk.bit_width = 32;
	fadt->x_pm_tmr_blk.bit_offset = 0;
	fadt->x_pm_tmr_blk.resv = 0;
	fadt->x_pm_tmr_blk.addrl = pmbase + 0x8;
	fadt->x_pm_tmr_blk.addrh = 0x0;
	fadt->x_gpe0_blk.space_id = 1;
	fadt->x_gpe0_blk.bit_width = 128;
	fadt->x_gpe0_blk.bit_offset = 0;
	fadt->x_gpe0_blk.resv = 0;
	fadt->x_gpe0_blk.addrl = pmbase + 0x20;
	fadt->x_gpe0_blk.addrh = 0x0;
	fadt->x_gpe1_blk.space_id = 0;
	fadt->x_gpe1_blk.bit_width = 0;
	fadt->x_gpe1_blk.bit_offset = 0;
	fadt->x_gpe1_blk.resv = 0;
	fadt->x_gpe1_blk.addrl = 0x0;
	fadt->x_gpe1_blk.addrh = 0x0;

	header->checksum =
	    table_compute_checksum((void *) fadt, header->length);

}

unsigned long acpi_fill_madt(unsigned long current)
{
        /* create all subtables for processors */
        current = acpi_create_madt_lapics(current);

        /* Write SB800 IOAPIC, only one */
        current += acpi_create_madt_ioapic((struct acpi_madt_ioapic *) current, 2, IO_APIC_ADDR, 0);
        current += acpi_create_madt_irqoverride((struct acpi_madt_irqoverride *) current, 0, 0, 2, 0);
        current += acpi_create_madt_irqoverride((struct acpi_madt_irqoverride *) current, 0, 9, 9, 0xD);
        current += acpi_create_madt_irqoverride((struct acpi_madt_irqoverride *) current, 0, 0xB, 0xB, 0xD);
        acpi_create_madt_lapic_nmi((struct acpi_madt_lapic_nmi *)current, 0, 0, 0);

	return current;
        /* 0: mean bus 0--->ISA */
        /* 0: PIC 0 */
        /* 2: APIC 2 */
        /* 5 mean: 0101 --> Edge-triggered, Active high */

}

unsigned long acpi_fill_mcfg(unsigned long current)
{
	pci_dev_t dev;
	struct pci_device_id id[] = { { 0x8086, 0x29c0 } };
        u32 reg;

	dev = pci_find_devices(id, 0);
 	if (dev == -1) 
                return current;
	//function does not rreach here due to wrong value of id calculated above.
        pci_read_config_dword(dev, 0x60, &reg);
        if ((reg & 0x07) != 0x01)  // require enabled + 256MB size
                return current;

	 current += acpi_create_mcfg_mmconfig((struct acpi_mcfg_mmconfig *) current,
                                             reg & 0xf0000000, 0x0, 0x0, 255);

        return current;
}
