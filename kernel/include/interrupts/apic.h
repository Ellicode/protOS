/**
 * @file        apic.h
 * @brief       APIC header file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
*/

typedef struct madt_header_t {
    sdt_header_t    header;
    uint32_t        lapic_address;
    uint32_t        flags;
} __attribute__ ((packed)) madt_header_t;

typedef struct madt_entry_header_t {
    uint8_t type;
    uint8_t length;
} __attribute__ ((packed)) madt_entry_header_t;

void apic_init();