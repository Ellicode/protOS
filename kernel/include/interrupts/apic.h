/**
 * @file        apic.h
 * @brief       APIC header file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
*/

#pragma once
#include "cpu/acpi.h"
typedef struct madt_header_t {
    sdt_header_t    header;
    uint32_t        lapic_address;
    uint32_t        flags;
} __attribute__ ((packed)) madt_header_t;

typedef struct madt_entry_header_t {
    uint8_t type;
    uint8_t length;
} __attribute__ ((packed)) madt_entry_header_t;

enum {
    MADT_ENTRY_LAPIC                = 0,
    MADT_ENTRY_IOAPIC               = 1,
    MADT_ENTRY_IOAPIC_OVERRIDE      = 2,
    MADT_ENTRY_IOAPIC_NMI_SOURCE    = 3,
    MADT_ENTRY_LAPIC_NMI            = 4,
    MADT_ENTRY_LAPIC_OVERRIDE       = 5,
    MADT_ENTRY_x2APIC               = 9,
};
typedef struct madt_ioapic_entry {
    madt_entry_header_t header;
    uint8_t             id;
    uint8_t             reserved;
    uint32_t            address;
    uint16_t            flags;
} __attribute__ ((packed)) madt_ioapic_entry;

#define PIC_COMMAND_MASTER  0x20
#define PIC_DATA_MASTER     0x21

#define PIC_COMMAND_SLAVE   0xA0
#define PIC_DATA_SLAVE      0xA1

#define ICW_1               0x11
#define ICW_2_M             0x20
#define ICW_2_S             0x28
#define ICW_3_M             0x04
#define ICW_3_S             0x02
#define ICW_4               0x01

void apic_init();