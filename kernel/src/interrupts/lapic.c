/**
 * @file        lapic.c
 * @brief       LAPIC source file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
*/

#include "debug/logger.h"
#include "interrupts/lapic.h"
#include "mem/vmm.h"
#include "boot.h"

void *g_lapic_base;

void lvt_set_entry(int reg, uint8_t vector) {
    if (reg < LAPIC_REG_LVT_TIMER || reg > LAPIC_REG_LVT_ERROR) { return; }
    uint32_t entry = (vector & 0xFFU);
    LAPIC_WRITE_REG(reg, entry);
}

void lapic_timer_init() {
    LAPIC_WRITE_REG(LAPIC_REG_TIMER_DIVIDE_CONFIG, 3); // 16

    // ...

    lvt_set_entry(LAPIC_REG_LVT_TIMER, 32);
}

void lapic_init(const madt_header_t *madt) {    
    g_lapic_base = (void *)(uintptr_t)madt->lapic_address + g_boot_info.hhdm;
    vmm_map_range_phys(g_kern_pml4, g_lapic_base, (void *)(uintptr_t)madt->lapic_address, 0x3F0, VMM_WRITE);

    kdebug("LAPIC id=%d, address=0x%x, version=0x%x", LAPIC_READ_REG(LAPIC_REG_ID), madt->lapic_address, LAPIC_READ_REG(LAPIC_REG_VERSION));
}