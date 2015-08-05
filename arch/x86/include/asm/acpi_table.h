#include <linux/string.h>
#include <malloc.h>
#include <common.h>
#include <asm/post.h>

#define RSDP_SIG		"RSD PTR "  /* RSDT pointer signature */
#define ACPI_TABLE_CREATOR	"UBOOT   "  /* Must be exactly 8 bytes long! */
#define OEM_ID			"UBOOT "    /* Must be exactly 6 bytes long! */
#define ASLC			"UBOO"      /* Must be exactly 4 bytes long! */

#define APM_CNT         0xb2
#define APM_CNT_CST_CONTROL     0x85
#define APM_CNT_PST_CONTROL     0x80
#define APM_CNT_ACPI_DISABLE    0x1e
#define APM_CNT_ACPI_ENABLE     0xe1
#define APM_CNT_MBI_UPDATE      0xeb
#define APM_CNT_GNVS_UPDATE     0xea
#define APM_CNT_FINALIZE        0xcb
#define APM_CNT_LEGACY          0xcc
#define APM_STS         0xb3 


#define MP_IRQ_POLARITY_DEFAULT 0x0
#define MP_IRQ_POLARITY_HIGH    0x1
#define MP_IRQ_POLARITY_LOW     0x3
#define MP_IRQ_POLARITY_MASK    0x3
#define MP_IRQ_TRIGGER_DEFAULT  0x0
#define MP_IRQ_TRIGGER_EDGE     0x4
#define MP_IRQ_TRIGGER_LEVEL    0xc
#define MP_IRQ_TRIGGER_MASK     0xc

#define ACTL                            0x00
# define SCIS_MASK                              0x07
# define SCIS_IRQ9                              0x00
# define SCIS_IRQ10                             0x01
# define SCIS_IRQ11                             0x02
# define SCIS_IRQ20                             0x04
# define SCIS_IRQ21                             0x05
# define SCIS_IRQ22                             0x06
# define SCIS_IRQ23                             0x07

#define ILB_BASE_ADDRESS                0xfed08000
#define ILB_BASE_SIZE                   0x400

enum bus_type {
        PIC =  0,
        APIC = 2,
        ETHIGH = 5
};


typedef struct acpi_gen_regaddr {
        u8  space_id;           /* Address space ID */
        u8  bit_width;          /* Register size in bits */
        u8  bit_offset;         /* Register bit offset */
        union {
                u8  resv;                       /* Reserved in ACPI 2.0 - 2.0b */
                u8  access_size;        /* Access size in ACPI 2.0c/3.0/4.0/5.0 */
        };
        u32 addrl;              /* Register address, low 32 bits */
        u32 addrh;              /* Register address, high 32 bits */
} __attribute__ ((packed)) acpi_addr_t;


/* RSDP (Root System Description Pointer) */
struct __packed acpi_rsdp {
	char  signature[8];	/* RSDP signature */
	u8    checksum;		/* Checksum of the first 20 bytes */
	char  oem_id[6];	/* OEM ID */
	u8    revision;		/* 0 for ACPI 1.0, 2 for ACPI 2.0/3.0/4.0 */
	u32   rsdt_address;	/* Physical address of RSDT (32 bits) */
	u32   length;		/* Total RSDP length (incl. extended part) */
	u64   xsdt_address;	/* Physical address of XSDT (64 bits) */
	u8    ext_checksum;	/* Checksum of the whole table */
	u8    reserved[3];
};
/* Note: ACPI 1.0 didn't have length, xsdt_address, and ext_checksum. */

enum acpi_address_space_type {
	ACPI_ADDRESS_SPACE_MEMORY = 0,	/* System memory */
	ACPI_ADDRESS_SPACE_IO = 1,	/* System I/O */
	ACPI_ADDRESS_SPACE_PCI = 2,	/* PCI config space */
	ACPI_ADDRESS_SPACE_EC = 3,	/* Embedded controller */
	ACPI_ADDRESS_SPACE_SMBUS = 4,	/* SMBus */
	ACPI_ADDRESS_SPACE_PCC = 0x0A,	/* Platform Comm. Channel */
	ACPI_ADDRESS_SPACE_FIXED = 0x7f	/* Functional fixed hardware */
};

#define  ACPI_FFIXEDHW_VENDOR_INTEL	   1	/* Intel */
#define  ACPI_FFIXEDHW_CLASS_HLT	   0	/* C1 Halt */
#define  ACPI_FFIXEDHW_CLASS_IO_HLT	   1	/* C1 I/O then Halt */
#define  ACPI_FFIXEDHW_CLASS_MWAIT	   2	/* MWAIT Native C-state */
#define  ACPI_FFIXEDHW_FLAG_HW_COORD	   1	/* Hardware Coordination bit */
#define  ACPI_FFIXEDHW_FLAG_BM_STS	   2	/* BM_STS avoidance bit */
/* 0x80-0xbf: Reserved */
/* 0xc0-0xff: OEM defined */

/* Access size definitions for Generic address structure */
enum acpi_address_space_size {
	ACPI_ACCESS_SIZE_UNDEFINED = 0,	/* Undefined (legacy reasons) */
	ACPI_ACCESS_SIZE_BYTE_ACCESS = 1,
	ACPI_ACCESS_SIZE_WORD_ACCESS = 2,
	ACPI_ACCESS_SIZE_DWORD_ACCESS = 3,
	ACPI_ACCESS_SIZE_QWORD_ACCESS = 4
};

/* Generic ACPI header, provided by (almost) all tables */
typedef struct acpi_table_header {
	char signature[4];           /* ACPI signature (4 ASCII characters) */
	u32  length;                 /* Table length in bytes (incl. header) */
	u8   revision;               /* Table version (not ACPI version!) */
	u8   checksum;               /* To make sum of entire table == 0 */
	char oem_id[6];              /* OEM identification */
	char oem_table_id[8];        /* OEM table identification */
	u32  oem_revision;           /* OEM revision number */
	char asl_compiler_id[4];     /* ASL compiler vendor ID */
	u32  asl_compiler_revision;  /* ASL compiler revision number */
} __attribute__ ((packed)) acpi_header_t;

/* A maximum number of 32 ACPI tables ought to be enough for now. */
#define MAX_ACPI_TABLES 32

/* RSDT (Root System Description Table) */
struct __packed acpi_rsdt {
	struct acpi_table_header header;
	u32 entry[MAX_ACPI_TABLES];
};

/* XSDT (Extended System Description Table) */
struct __packed acpi_xsdt {
	struct acpi_table_header header;
	u64 entry[MAX_ACPI_TABLES];
};

/* MCFG (PCI Express MMIO config space BAR description table) */
struct __packed acpi_mcfg {
	struct acpi_table_header header;
	u8 reserved[8];
};

struct __packed acpi_mcfg_mmconfig {
	u32 base_address;
	u32 base_reserved;
	u16 pci_segment_group_number;
	u8 start_bus_number;
	u8 end_bus_number;
	u8 reserved[4];
};

/* MADT (Multiple APIC Description Table) */
struct __packed acpi_madt {
	struct acpi_table_header header;
	u32 lapic_addr;			/* Local APIC address */
	u32 flags;			/* Multiple APIC flags */
} __attribute__ ((packed)) acpi_madt_t;

enum dev_scope_type {
	SCOPE_PCI_ENDPOINT = 1,
	SCOPE_PCI_SUB = 2,
	SCOPE_IOAPIC = 3,
	SCOPE_MSI_HPET = 4
};

typedef struct dev_scope {
	u8 type;
	u8 length;
	u8 reserved[2];
	u8 enumeration;
	u8 start_bus;
	struct {
		u8 dev;
		u8 fn;
	} __attribute__((packed)) path[0];
} __attribute__ ((packed)) dev_scope_t;

/* MADT: APIC Structure Types */
/* TODO: Convert to ALLCAPS. */
enum acpi_apic_types {
	LocalApic		= 0,	/* Processor local APIC */
	IOApic			= 1,	/* I/O APIC */
	IRQSourceOverride	= 2,	/* Interrupt source override */
	NMIType			= 3,	/* NMI source */
	LocalApicNMI		= 4,	/* Local APIC NMI */
	LApicAddressOverride	= 5,	/* Local APIC address override */
	IOSApic			= 6,	/* I/O SAPIC */
	LocalSApic		= 7,	/* Local SAPIC */
	PlatformIRQSources	= 8,	/* Platform interrupt sources */
	Localx2Apic		= 9,	/* Processor local x2APIC */
	Localx2ApicNMI		= 10,	/* Local x2APIC NMI */
	/* 0x0b-0x7f: Reserved */
	/* 0x80-0xff: Reserved for OEM use */
};

/* MADT: Processor Local APIC Structure */
struct __packed acpi_madt_lapic {
	u8 type;			/* Type (0) */
	u8 length;			/* Length in bytes (8) */
	u8 processor_id;		/* ACPI processor ID */
	u8 apic_id;			/* Local APIC ID */
	u32 flags;			/* Local APIC flags */
};

/* MADT: Local APIC NMI Structure */
struct __packed acpi_madt_lapic_nmi {
	u8 type;			/* Type (4) */
	u8 length;			/* Length in bytes (6) */
	u8 processor_id;		/* ACPI processor ID */
	u16 flags;			/* MPS INTI flags */
	u8 lint;			/* Local APIC LINT# */
};

/* MADT: I/O APIC Structure */
struct __packed acpi_madt_ioapic {
	u8 type;			/* Type (1) */
	u8 length;			/* Length in bytes (12) */
	u8 ioapic_id;			/* I/O APIC ID */
	u8 reserved;
	u32 ioapic_addr;		/* I/O APIC address */
	u32 gsi_base;			/* Global system interrupt base */
};

/* MADT: Interrupt Source Override Structure */
struct __packed acpi_madt_irqoverride {
	u8 type;			/* Type (2) */
	u8 length;			/* Length in bytes (10) */
	u8 bus;				/* ISA (0) */
	u8 source;			/* Bus-relative int. source (IRQ) */
	u32 gsirq;			/* Global system interrupt */
	u16 flags;			/* MPS INTI flags */
};

/* FADT (Fixed ACPI Description Table) */
struct __packed acpi_fadt {
	struct acpi_table_header header;
	u32 firmware_ctrl;
	u32 dsdt;
	u8 model;
	u8 preferred_pm_profile;
	u16 sci_int;
	u32 smi_cmd;
	u8 acpi_enable;
	u8 acpi_disable;
	u8 s4bios_req;
	u8 pstate_cnt;
	u32 pm1a_evt_blk;
	u32 pm1b_evt_blk;
	u32 pm1a_cnt_blk;
	u32 pm1b_cnt_blk;
	u32 pm2_cnt_blk;
	u32 pm_tmr_blk;
	u32 gpe0_blk;
	u32 gpe1_blk;
	u8 pm1_evt_len;
	u8 pm1_cnt_len;
	u8 pm2_cnt_len;
	u8 pm_tmr_len;
	u8 gpe0_blk_len;
	u8 gpe1_blk_len;
	u8 gpe1_base;
	u8 cst_cnt;
	u16 p_lvl2_lat;
	u16 p_lvl3_lat;
	u16 flush_size;
	u16 flush_stride;
	u8 duty_offset;
	u8 duty_width;
	u8 day_alrm;
	u8 mon_alrm;
	u8 century;
	u16 iapc_boot_arch;
	u8 res2;
	u32 flags;
	struct acpi_gen_regaddr reset_reg;
	u8 reset_value;
	u8 res3;
	u8 res4;
	u8 res5;
	u32 x_firmware_ctl_l;
	u32 x_firmware_ctl_h;
	u32 x_dsdt_l;
	u32 x_dsdt_h;
	struct acpi_gen_regaddr x_pm1a_evt_blk;
	struct acpi_gen_regaddr x_pm1b_evt_blk;
	struct acpi_gen_regaddr x_pm1a_cnt_blk;
	struct acpi_gen_regaddr x_pm1b_cnt_blk;
	struct acpi_gen_regaddr x_pm2_cnt_blk;
	struct acpi_gen_regaddr x_pm_tmr_blk;
	struct acpi_gen_regaddr x_gpe0_blk;
	struct acpi_gen_regaddr x_gpe1_blk;
};

/* FADT TABLE Revision values */
#define ACPI_FADT_REV_ACPI_1_0		1
#define ACPI_FADT_REV_ACPI_2_0		3
#define ACPI_FADT_REV_ACPI_3_0		4
#define ACPI_FADT_REV_ACPI_4_0		4
#define ACPI_FADT_REV_ACPI_5_0		5

/* Flags for p_lvl2_lat and p_lvl3_lat */
#define ACPI_FADT_C2_NOT_SUPPORTED	101
#define ACPI_FADT_C3_NOT_SUPPORTED	1001

/* FADT Feature Flags */
#define ACPI_FADT_WBINVD		(1 << 0)
#define ACPI_FADT_WBINVD_FLUSH		(1 << 1)
#define ACPI_FADT_C1_SUPPORTED		(1 << 2)
#define ACPI_FADT_C2_MP_SUPPORTED	(1 << 3)
#define ACPI_FADT_POWER_BUTTON		(1 << 4)
#define ACPI_FADT_SLEEP_BUTTON		(1 << 5)
#define ACPI_FADT_FIXED_RTC		(1 << 6)
#define ACPI_FADT_S4_RTC_WAKE		(1 << 7)
#define ACPI_FADT_32BIT_TIMER		(1 << 8)
#define ACPI_FADT_DOCKING_SUPPORTED	(1 << 9)
#define ACPI_FADT_RESET_REGISTER	(1 << 10)
#define ACPI_FADT_SEALED_CASE		(1 << 11)
#define ACPI_FADT_HEADLESS		(1 << 12)
#define ACPI_FADT_SLEEP_TYPE		(1 << 13)
#define ACPI_FADT_PCI_EXPRESS_WAKE	(1 << 14)
#define ACPI_FADT_PLATFORM_CLOCK	(1 << 15)
#define ACPI_FADT_S4_RTC_VALID		(1 << 16)
#define ACPI_FADT_REMOTE_POWER_ON	(1 << 17)
#define ACPI_FADT_APIC_CLUSTER		(1 << 18)
#define ACPI_FADT_APIC_PHYSICAL		(1 << 19)
/* Bits 20-31: reserved ACPI 3.0 & 4.0 */
#define ACPI_FADT_HW_REDUCED_ACPI	(1 << 20)
#define ACPI_FADT_LOW_PWR_IDLE_S0	(1 << 21)
/* bits 22-31: reserved ACPI 5.0 */

/* FADT Boot Architecture Flags */
enum acpi_fadt_boot_flags {
	ACPI_FADT_LEGACY_DEVICES = (1 << 0),
	ACPI_FADT_8042 = (1 << 1),
	ACPI_FADT_VGA_NOT_PRESENT = (1 << 2),
	ACPI_FADT_MSI_NOT_SUPPORTED = (1 << 3),
	ACPI_FADT_NO_PCIE_ASPM_CONTROL = (1 << 4),
	ACPI_FADT_LEGACY_FREE = 0x00	/* No legacy devices (including 8042) */
};
/* FADT Preferred Power Management Profile */
enum acpi_preferred_pm_profiles {
	PM_UNSPECIFIED		= 0,
	PM_DESKTOP		= 1,
	PM_MOBILE		= 2,
	PM_WORKSTATION		= 3,
	PM_ENTERPRISE_SERVER	= 4,
	PM_SOHO_SERVER  	= 5,
	PM_APPLIANCE_PC		= 6,
	PM_PERFORMANCE_SERVER	= 7,
	PM_TABLET		= 8,	/* ACPI 5.0 */
};

/* FACS (Firmware ACPI Control Structure) */
struct __packed acpi_facs {
	char signature[4];			/* "FACS" */
	u32 length;				/* Length in bytes (>= 64) */
	u32 hardware_signature;			/* Hardware signature */
	u32 firmware_waking_vector;		/* Firmware waking vector */
	u32 global_lock;			/* Global lock */
	u32 flags;				/* FACS flags */
	u32 x_firmware_waking_vector_l;		/* X FW waking vector, low */
	u32 x_firmware_waking_vector_h;		/* X FW waking vector, high */
	u8 version;				/* ACPI 4.0: 2 */
	u8 resv[31];				/* FIXME: 4.0: ospm_flags */
};

/* FACS flags */
#define ACPI_FACS_S4BIOS_F	(1 << 0)
#define ACPI_FACS_64BIT_WAKE_F	(1 << 1)
/* Bits 31..2: reserved */

/* These can be used by the target port. */

void acpi_add_table(struct acpi_rsdp *rsdp, void *table);
unsigned long write_acpi_tables(unsigned long start);
int acpi_create_madt_lapic(struct acpi_madt_lapic *lapic, u8 cpu, u8 apic);
unsigned long acpi_create_madt_lapics(unsigned long current);
int acpi_create_madt_ioapic(struct acpi_madt_ioapic *ioapic, u8 id, u32 addr, u32 gsi_base);
int acpi_create_madt_irqoverride(struct acpi_madt_irqoverride *irqoverride, u8 bus, u8 source, u32 gsirq, u16 flags);
unsigned long acpi_fill_mcfg(unsigned long current);
unsigned long acpi_fill_madt(unsigned long current);
void acpi_create_fadt(struct acpi_fadt *fadt, struct acpi_facs *facs, void *dsdt);
int acpi_create_mcfg_mmconfig(struct acpi_mcfg_mmconfig *mmconfig, u32 base, u16 seg_nr, u8 start, u8 end);
int acpi_create_madt_lapic_nmi(struct acpi_madt_lapic_nmi *lapic_nmi, u8 cpu, u16 flags, u8 lint);
static inline uint32_t read32(const void *addr)
{
        return *(volatile uint32_t *)addr;
}

