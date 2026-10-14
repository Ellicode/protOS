/**
 * @file        apic.c
 * @brief       APIC source file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
*/
#include "debug/logger.h"
#include "cpu/acpi.h"
#include "std/string.h"
#include "interrupts/apic.h"
#include "boot.h"
#include "panic.h"

static madt_header_t *madt_fetch() {
    rsdp_descriptor_v1_t *rsdp = g_boot_info.rsdp;
    if (rsdp->revision == 2) {
        rsdp_descriptor_v2_t *rsdp_v2 = g_boot_info.rsdp;

        xsdt_header_t *xsdt = (xsdt_header_t *)(rsdp_v2->xsdt_address + g_boot_info.hhdm);
        if (!xsdt) {
            kerror("XSDT not found");
            return NULL;
        }

        size_t xsdt_entry_count = (xsdt->header.length - sizeof(sdt_header_t)) / 8;
        for (size_t i = 0; i < xsdt_entry_count; i++) {
            madt_header_t *entry = (madt_header_t *)(xsdt->sdtAddresses[i] + g_boot_info.hhdm);
            if (strncmp(entry->header.signature, "APIC", 4) == 0) {
                return entry;
            }
        }
        return NULL;
    } else if (rsdp->revision == 0) {
        rsdt_header_t *rsdt = (rsdt_header_t *)(rsdp->rsdt_address + g_boot_info.hhdm);
        if (!rsdt) {
            kerror("RSDT not found");
            return NULL;
        }

        size_t rsdt_entry_count = (rsdt->header.length - sizeof(sdt_header_t)) / 8;
        for (size_t i = 0; i < rsdt_entry_count; i++) {
            madt_header_t *entry = (madt_header_t *)(rsdt->sdtAddresses[i] + g_boot_info.hhdm);
            if (strncmp(entry->header.signature, "APIC", 4) == 0) {
                return entry;
            }
        }
        return NULL;
    }
    kerror("Unknown RSDP revision (%d)", rsdp->revision);
    return NULL;
}

static void madt_parse(madt_header_t *madt) {
    uint8_t *madt_base = (uint8_t *)madt;
    size_t offset = sizeof(madt_header_t);

    while (offset < madt->header.length) {
        madt_entry_header_t *entry_hdr = (madt_entry_header_t *)(madt_base + offset);
        if (entry_hdr->length == 0) { break; }
        kdebug("MADT entry type %d", entry_hdr->type);

        offset += entry_hdr->length;
    }    
}

void apic_init() {
    madt_header_t *madt = madt_fetch();

    if (madt == NULL) {
        kerror("MADT table not present");
        return;
    }

    kdebug("LAPIC address: 0x%x", madt->lapic_address);

    madt_parse(madt);
}