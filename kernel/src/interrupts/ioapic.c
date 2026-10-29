/**
 * @file        ioapic.c
 * @brief       IOAPIC source file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
*/

#include "debug/logger.h"
#include "interrupts/ioapic.h"
#include "mem/vmm.h"
#include "dt/idt.h"
#include "boot.h"

void *g_ioapic_base;
irq_override_t g_irq_overrides[IRQ_COUNT];

static uint32_t ioapic_read_reg(uint32_t reg) {
    uint32_t volatile *ioapic = (uint32_t volatile *)g_ioapic_base;
    ioapic[IOREGSEL] = (reg & 0xFFU);
    return ioapic[IOWIN];
}

static void ioapic_write_reg(uint32_t reg, uint32_t value) {
    uint32_t volatile *ioapic = (uint32_t volatile *)g_ioapic_base;
    ioapic[IOREGSEL] = (reg & 0xFFU);
    ioapic[IOWIN] = value;
}

void ioapic_parse_iso(const madt_iso_entry_t *entry) {
    if (entry->irq > IRQ_COUNT) { return; }
    g_irq_overrides[entry->irq] = (irq_override_t) {
        .gsi = entry->gsi,
        .flags = entry->flags,
    };
    kdebug("IRQ %d -> GSI %d", entry->irq, entry->gsi);
}

void ioapic_init(const madt_ioapic_entry_t *entry) {
    g_ioapic_base = (void *)(uintptr_t)entry->address + g_boot_info.hhdm;
    vmm_map_range_phys(g_kern_pml4, g_ioapic_base, (void *)(uintptr_t)entry->address, entry->header.length, VMM_WRITE);

    kdebug("IOAPIC id=%d, address=0x%x, version=0x%x", entry->id, entry->address, ioapic_read_reg(IOAPICVER));
}

void ioapic_set_entry() {
    
}

void ioapic_fill() {
    for (int irq = 0; irq < IRQ_COUNT; irq++) {
        g_irq_overrides[irq] = (irq_override_t) {
            .gsi = irq,
            .flags = 0,
        };
    }

    size_t max_redirections = ((ioapic_read_reg(IOAPICVER) >> 16U) & 0xFFU) + 1;
    kdebug("IOAPIC max redirections: %d", max_redirections);


}