/**
 * @file        acpi.c
 * @brief       ACPI source file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
*/
#include "debug/logger.h"
#include "cpu/acpi.h"
#include "std/string.h"
#include "boot.h"
#include "panic.h"

void acpi_init() {
    rsdp_descriptor_v1_t *rsdp = g_boot_info.rsdp;
    rsdp->signature[7] = '\0';
    if (strncmp(rsdp->signature, "RSD PTR", 7) != 0) {
        kerror("Invalid RSDP signature");
        return;
    }

    kdebug("RSDP: { signature: \"%s\", checksum: %d, oemid: 0x%x, revision: %d, rsdt_addr: 0x%x }", rsdp->signature, rsdp->checksum, rsdp->oem_id, rsdp->revision, rsdp->rsdt_address);
    if (rsdp->revision == 2) {
        rsdp_descriptor_v2_t *rsdp_v2 = g_boot_info.rsdp;
        kdebug("RSDP (v2): { length: %d, xsdt_addr: 0x%x, extended_checksum: %d }", rsdp_v2->length, rsdp_v2->xsdt_address, rsdp_v2->extended_checksum);

        xsdt_header_t *xsdt = (xsdt_header_t *)(rsdp_v2->xsdt_address + g_boot_info.hhdm);

        if (!xsdt) {
            kerror("XSDT not found");
            return;
        }

        char xsdt_signature[5];;
        strncpy(xsdt_signature, xsdt->header.signature, 5);
        xsdt_signature[4] = '\0';
        
        kdebug("XSDT: { signature: \"%s\", length: %d, revision: %d, checksum: %d, oem_id: \"%s\", oem_table_id: \"%s\", oem_revision: %d, creator_id: %d, creator_revision: %d }", xsdt_signature, xsdt->header.length, xsdt->header.revision, xsdt->header.checksum, xsdt->header.oem_id, xsdt->header.oem_table_id, xsdt->header.oem_table_id, xsdt->header.creator_id, xsdt->header.creator_revision);

        size_t xsdt_entry_count = (xsdt->header.length - sizeof(sdt_header_t)) / 8;

        for (size_t i = 0; i < xsdt_entry_count; i++) {
            sdt_header_t *entry = (sdt_header_t *)(xsdt->sdtAddresses[i] + g_boot_info.hhdm);
            
            char entry_signature[5];
            strncpy(entry_signature, entry->signature, 5);
            entry_signature[4] = '\0';

            kdebug("XSDT[%d]: { signature: \"%s\", length: %d, revision: %d, checksum: %d, oem_id: \"%s\", oem_table_id: \"%s\", oem_revision: %d, creator_id: %d, creator_revision: %d }", i, entry_signature, entry->length, entry->revision, entry->checksum, entry->oem_id, entry->oem_table_id, entry->oem_table_id, entry->creator_id, entry->creator_revision);
        }
    }
}