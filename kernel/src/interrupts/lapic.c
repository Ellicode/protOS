/**
 * @file        lapic.c
 * @brief       LAPIC source file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
*/

#include "debug/logger.h"
#include "drivers/pit.h"
#include "interrupts/lapic.h"
#include "mem/vmm.h"
#include "utils/msr.h"
#include "boot.h"

void *g_lapic_base;
uint64_t g_lapic_timer_ticks = 0;

void lvt_set_entry(int reg, uint8_t vector) {
    if (reg < LAPIC_REG_LVT_TIMER || reg > LAPIC_REG_LVT_ERROR) { return; }
    uint32_t entry = (vector & 0xFFU);
    LAPIC_WRITE_REG(reg, entry);
}

void lapic_timer_init() {
    uint32_t timer_frequency = pit_calibrate();

    LAPIC_WRITE_REG(LAPIC_REG_TIMER_DIVIDE_CONFIG, 3); // 16
    if (timer_frequency == 0) {
        LAPIC_WRITE_REG(LAPIC_REG_LVT_TIMER, LVT_DISABLED | 32U);
        LAPIC_WRITE_REG(LAPIC_REG_TIMER_INITIAL_COUNT, 0);
        return;
    }

    uint32_t initial_count = timer_frequency / LAPIC_TIMER_HZ;
    if (initial_count == 0) {
        initial_count = 1;
    }

    LAPIC_WRITE_REG(LAPIC_REG_LVT_TIMER, LVT_DISABLED | 32U | (1U << 17U));
    LAPIC_WRITE_REG(LAPIC_REG_TIMER_INITIAL_COUNT, initial_count);
    LAPIC_WRITE_REG(LAPIC_REG_LVT_TIMER, 32U | (1U << 17U));
}

void lapic_eoi(void) {
    LAPIC_WRITE_REG(LAPIC_REG_EOI, 0);
}

void lapic_init(const madt_header_t *madt) {    
    g_lapic_base = (void *)(uintptr_t)madt->lapic_address + g_boot_info.hhdm;
    vmm_map_range_phys(g_kern_pml4, g_lapic_base, (void *)(uintptr_t)madt->lapic_address, 0x3F0, VMM_WRITE);

    wrmsr(0x1B, rdmsr(0x1B) | (1ULL << 11U));
    LAPIC_WRITE_REG(LAPIC_REG_TPR, 0);
    LAPIC_WRITE_REG(LAPIC_REG_SPURIOUS_VECTOR,
        (LAPIC_READ_REG(LAPIC_REG_SPURIOUS_VECTOR) & ~0x1FFU) | 0x1FFU);

    kdebug("LAPIC id=%d, address=0x%x, version=0x%x", LAPIC_READ_REG(LAPIC_REG_ID), madt->lapic_address, LAPIC_READ_REG(LAPIC_REG_VERSION));
}