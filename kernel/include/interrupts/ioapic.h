/**
 * @file        ioapic.h
 * @brief       IOAPIC header file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
*/

#pragma once
#include "interrupts/apic.h"

extern void *g_ioapic_base;

#define IOAPIC_REDIR_BASE       0x10U
#define IOAPIC_REDIR_MASKED     (1U << 16U)
#define IOAPIC_REDIR_POLARITY   (1U << 13U)
#define IOAPIC_REDIR_TRIGGER    (1U << 15U)

// IOAPIC registers
enum {
    IOAPICID    = 0, // RW
    IOAPICVER   = 1, // R
    IOAPICARB   = 2, // R
    IOREDTBL    = 3, // RW
};
typedef struct irq_override_t {
    uint32_t gsi;
    uint16_t flags;
} irq_override_t;

#define IOREGSEL    0
#define IOWIN       4

void ioapic_parse_iso(const madt_iso_entry_t *entry);
void ioapic_init(const madt_ioapic_entry_t *entry);
void ioapic_init_overrides();
int ioapic_set_irq_route(uint8_t irq, uint8_t vector, int masked);
void ioapic_fill();