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
#if 0
static inline uint32_t read32(const void *addr)
{
        dmb();
        return *(volatile uint32_t *)addr;
}
#endif
static int acpi_sci_irq(void)
{
        u32 *actl = (u32 *)(ILB_BASE_ADDRESS + ACTL);
        int scis;
        static int sci_irq;

        if (sci_irq)
                return sci_irq;

        /* Determine how SCI is routed. */
        scis = read32(actl) & SCIS_MASK;
        switch (scis) {
        case SCIS_IRQ9:
        case SCIS_IRQ10:
        case SCIS_IRQ11:
                sci_irq = scis - SCIS_IRQ9 + 9;
                break;
        case SCIS_IRQ20:
        case SCIS_IRQ21:
        case SCIS_IRQ22:
        case SCIS_IRQ23:
                sci_irq = scis - SCIS_IRQ20 + 20;
                break;
        default:
                printf("Invalid SCI route! Defaulting to IRQ9.\n");
                sci_irq = 9;
                break;
        }

        printf("SCI is IRQ%d\n", sci_irq);
        return sci_irq;

}

unsigned long acpi_madt_irq_overrides(unsigned long current)
{
	 int sci_irq = acpi_sci_irq();
        struct acpi_madt_irqoverride *irqovr;
        uint16_t sci_flags = MP_IRQ_TRIGGER_LEVEL;

        /* INT_SRC_OVR */
        irqovr = (void *)current;
        current += acpi_create_madt_irqoverride(irqovr, 0, 0, 2, 0);

        if (sci_irq >= 20)
                sci_flags |= MP_IRQ_POLARITY_LOW;
        else
                sci_flags |= MP_IRQ_POLARITY_HIGH;

        irqovr = (void *)current;
        current += acpi_create_madt_irqoverride(irqovr, 0, sci_irq, sci_irq,
                                                sci_flags);

        return current;

}

void acpi_create_fadt(struct acpi_fadt * fadt, struct acpi_facs * facs, void *dsdt)
{
	acpi_header_t *header = &(fadt->header);
        u16 pm, pmbase;

        pci_dev_t bdf = PCI_BDF(0, 0x1f, 0);
        pci_read_config_word(bdf, 0x40, &pm);
        pmbase = pm & 0xfffe;
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
#if 1
        fadt->smi_cmd = 0;//APM_CNT;
        fadt->acpi_enable = 0;//APM_CNT_ACPI_ENABLE;
        fadt->acpi_disable = 0;//APM_CNT_ACPI_DISABLE;
#endif
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
        fadt->pm1_cnt_len = 2; 
        fadt->pm2_cnt_len = 1;
        fadt->pm_tmr_len = 4;
        fadt->gpe0_blk_len = 16;
        fadt->gpe1_blk_len = 0;
        fadt->gpe1_base = 0;
        fadt->cst_cnt = 0;
#if 1
        fadt->p_lvl3_lat = 87;
        fadt->flush_size = 1024;
        fadt->flush_stride = 16;
        fadt->duty_offset = 1;
        fadt->day_alrm = 0xd;
        fadt->mon_alrm = 0x00;
        fadt->century = 0x00;
        fadt->iapc_boot_arch = ACPI_FADT_LEGACY_DEVICES | ACPI_FADT_8042;
#endif

        fadt->flags = ACPI_FADT_WBINVD |
                        ACPI_FADT_C1_SUPPORTED |
                        ACPI_FADT_SLEEP_BUTTON |
                        ACPI_FADT_RESET_REGISTER |
                        ACPI_FADT_SEALED_CASE |
                        ACPI_FADT_S4_RTC_WAKE |
                        ACPI_FADT_PLATFORM_CLOCK;
        fadt->reset_reg.space_id = 1;
        fadt->reset_reg.bit_width = 8;
        fadt->reset_reg.bit_offset = 0;
        fadt->reset_reg.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS;
        fadt->reset_reg.addrl = 0xcf9;
        fadt->reset_reg.addrh = 0;
        fadt->reset_value = 6;
        fadt->x_firmware_ctl_l = (unsigned long)facs;
        fadt->x_firmware_ctl_h = 0;
        fadt->x_dsdt_l = (unsigned long)dsdt;
        fadt->x_dsdt_h = 0;

        fadt->x_pm1a_evt_blk.space_id = 1;
        fadt->x_pm1a_evt_blk.bit_width = 32;
        fadt->x_pm1a_evt_blk.bit_offset = 0;
        fadt->x_pm1a_evt_blk.access_size = ACPI_ACCESS_SIZE_DWORD_ACCESS;
        fadt->x_pm1a_evt_blk.addrl = pmbase;
        fadt->x_pm1a_evt_blk.addrh = 0x0;

        fadt->x_pm1b_evt_blk.space_id = 1;
        fadt->x_pm1b_evt_blk.bit_width = 0;
        fadt->x_pm1b_evt_blk.bit_offset = 0;
        fadt->x_pm1b_evt_blk.access_size = 0;
        fadt->x_pm1b_evt_blk.addrl = 0x0;
        fadt->x_pm1b_evt_blk.addrh = 0x0;

        fadt->x_pm1a_cnt_blk.space_id = 1;
        fadt->x_pm1a_cnt_blk.bit_width = 16;
        fadt->x_pm1a_cnt_blk.bit_offset = 0;
        fadt->x_pm1a_cnt_blk.access_size = ACPI_ACCESS_SIZE_WORD_ACCESS;
        fadt->x_pm1a_cnt_blk.addrl = pmbase + 0x4;
        fadt->x_pm1a_cnt_blk.addrh = 0x0;

        fadt->x_pm1b_cnt_blk.space_id = 1;
        fadt->x_pm1b_cnt_blk.bit_width = 0;
        fadt->x_pm1b_cnt_blk.bit_offset = 0;
        fadt->x_pm1b_cnt_blk.access_size = 0;
        fadt->x_pm1b_cnt_blk.addrl = 0x0;
        fadt->x_pm1b_cnt_blk.addrh = 0x0;

        fadt->x_pm2_cnt_blk.space_id = 1;
        fadt->x_pm2_cnt_blk.bit_width = 8;
        fadt->x_pm2_cnt_blk.bit_offset = 0;
        fadt->x_pm2_cnt_blk.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS;
        fadt->x_pm2_cnt_blk.addrl = pmbase + 0x50;
        fadt->x_pm2_cnt_blk.addrh = 0x0;

        fadt->x_pm_tmr_blk.space_id = 1;
        fadt->x_pm_tmr_blk.bit_width = 32;
        fadt->x_pm_tmr_blk.bit_offset = 0;
        fadt->x_pm_tmr_blk.access_size = ACPI_ACCESS_SIZE_DWORD_ACCESS;
        fadt->x_pm_tmr_blk.addrl = pmbase + 0x8;
        fadt->x_pm_tmr_blk.addrh = 0x0;

        fadt->x_gpe0_blk.space_id = 1;
        fadt->x_gpe0_blk.bit_width = 128;
        fadt->x_gpe0_blk.bit_offset = 0;
        fadt->x_gpe0_blk.access_size = ACPI_ACCESS_SIZE_DWORD_ACCESS;
        fadt->x_gpe0_blk.addrl = pmbase + 0x20;
        fadt->x_gpe0_blk.addrh = 0x0;

        fadt->x_gpe1_blk.space_id = 1;
        fadt->x_gpe1_blk.bit_width = 0;
        fadt->x_gpe1_blk.bit_offset = 0;
        fadt->x_gpe1_blk.access_size = 0;
        fadt->x_gpe1_blk.addrl = 0x0;
        fadt->x_gpe1_blk.addrh = 0x0;


        header->checksum =
            table_compute_checksum((void *) fadt, header->length);

}

unsigned long acpi_fill_madt(unsigned long current)
{
        current = acpi_create_madt_lapics(current);

        /* IOAPIC */
        current += acpi_create_madt_ioapic((struct acpi_madt_ioapic *) current,
                                2, IO_APIC_ADDR, 0);

        current = acpi_madt_irq_overrides(current);

        return current;

}

unsigned long acpi_fill_mcfg(unsigned long current)
{
        return current;
}
