/**
 * @file        ioapic.c
 * @brief       IOAPIC source file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
*/

#include "debug/logger.h"
#include "interrupts/ioapic.h"

void *g_ioapic_base;

void ioapic_init(madt_ioapic_entry *entry) {
    g_ioapic_base = (void *)(uintptr_t)entry->address;
    kdebug("IOAPIC id=%d, address=0x%x", entry->id, entry->address);
}