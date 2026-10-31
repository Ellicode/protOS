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
#include "interrupts/lapic.h"
#include "boot.h"

void *g_ioapic_base;
irq_override_t g_irq_overrides[IRQ_COUNT];
static uint32_t g_ioapic_gsi_base;

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

void ioapic_init_overrides(void) {
    for (uint32_t irq = 0; irq < IRQ_COUNT; irq++) {
        g_irq_overrides[irq] = (irq_override_t) {
            .gsi = irq,
            .flags = 0,
        };
    }
}

void ioapic_parse_iso(const madt_iso_entry_t *entry) {
    if (entry->irq >= IRQ_COUNT) { return; }
    g_irq_overrides[entry->irq] = (irq_override_t) {
        .gsi = entry->gsi,
        .flags = entry->flags,
    };
    kdebug("IRQ %d -> GSI %d", entry->irq, entry->gsi);
}

void ioapic_init(const madt_ioapic_entry_t *entry) {
    g_ioapic_gsi_base = entry->gsi_base;
    g_ioapic_base = (void *)(uintptr_t)entry->address + g_boot_info.hhdm;
    vmm_map_range_phys(g_kern_pml4, g_ioapic_base, (void *)(uintptr_t)entry->address, 0x20, VMM_WRITE);

    kdebug("IOAPIC id=%d, address=0x%x, version=0x%x", entry->id, entry->address, ioapic_read_reg(IOAPICVER));
}

static int ioapic_get_irq_for_gsi(uint32_t gsi) {
    for (uint32_t irq = 0; irq < IRQ_COUNT; irq++) {
        if (g_irq_overrides[irq].gsi == gsi) {
            return (int)irq;
        }
    }
    return -1;
}

static int ioapic_get_redirection_flags(uint16_t flags, uint32_t *redir_flags) {
    uint32_t flag_bits = flags;
    uint32_t polarity = flag_bits & 0x3U;
    uint32_t trigger = (flag_bits >> 2U) & 0x3U;

    if (polarity == 0x2U || trigger == 0x2U) {
        return 0;
    }

    *redir_flags = 0;
    if (polarity == 0x3U) {
        *redir_flags |= IOAPIC_REDIR_POLARITY;
    }
    if (trigger == 0x3U) {
        *redir_flags |= IOAPIC_REDIR_TRIGGER;
    }
    return 1;
}

int ioapic_set_irq_route(uint8_t irq, uint8_t vector, int masked) {
    if (g_ioapic_base == NULL || irq >= IRQ_COUNT) {
        return 0;
    }

    uint32_t gsi = g_irq_overrides[irq].gsi;
    if (gsi < g_ioapic_gsi_base) {
        return 0;
    }

    uint32_t pin = gsi - g_ioapic_gsi_base;
    size_t max_redirections = ((ioapic_read_reg(IOAPICVER) >> 16U) & 0xFFU) + 1U;
    if (pin >= max_redirections) {
        return 0;
    }

    uint32_t redir_flags;
    if (!ioapic_get_redirection_flags(g_irq_overrides[irq].flags, &redir_flags)) {
        return 0;
    }

    uint32_t reg = IOAPIC_REDIR_BASE + (pin * 2U);
    uint32_t low = (uint32_t)vector | redir_flags;
    if (masked) {
        low |= IOAPIC_REDIR_MASKED;
    }

    uint32_t apic_id = (LAPIC_READ_REG(LAPIC_REG_ID) >> 24U) & 0xFFU;
    uint32_t current_low = ioapic_read_reg(reg);
    ioapic_write_reg(reg, current_low | IOAPIC_REDIR_MASKED);
    ioapic_write_reg(reg + 1U, apic_id << 24U);
    ioapic_write_reg(reg, low);
    return 1;
}

void ioapic_fill() {
    if (g_ioapic_base == NULL) {
        return;
    }

    size_t max_redirections = ((ioapic_read_reg(IOAPICVER) >> 16U) & 0xFFU) + 1;
    kdebug("IOAPIC max redirections: %d", max_redirections);

    uint32_t apic_id = (LAPIC_READ_REG(LAPIC_REG_ID) >> 24U) & 0xFFU;
    for (uint32_t pin = 0; pin < max_redirections; pin++) {
        uint32_t gsi = g_ioapic_gsi_base + pin;
        int irq = ioapic_get_irq_for_gsi(gsi);
        uint32_t low = IOAPIC_REDIR_MASKED;
        uint32_t high = apic_id << 24U;

        if (irq >= 0) {
            uint32_t redir_flags;
            if (ioapic_get_redirection_flags(g_irq_overrides[irq].flags, &redir_flags)) {
                low |= (uint32_t)(32 + irq) | redir_flags;
            } else {
                kerror("Invalid MADT flags for IRQ %d", irq);
            }
        }

        uint32_t reg = IOAPIC_REDIR_BASE + (pin * 2U);
        uint32_t current_low = ioapic_read_reg(reg);
        ioapic_write_reg(reg, current_low | IOAPIC_REDIR_MASKED);
        ioapic_write_reg(reg + 1U, high);
        ioapic_write_reg(reg, low);
    }

}