/**
 * @file        lapic.h
 * @brief       LAPIC header file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
*/

#pragma once
#include <stdint.h>
#include "interrupts/apic.h"

extern void *g_lapic_base;
extern uint64_t g_lapic_timer_ticks;

// LAPIC registers (https://wiki.osdev.org/APIC)
enum {
    LAPIC_REG_ID                    = 0x20, // RW
    LAPIC_REG_VERSION               = 0x30, // R
    LAPIC_REG_TPR                   = 0x80, // RW
    LAPIC_REG_APR                   = 0x90, // R
    LAPIC_REG_PPR                   = 0xA0, // R
    LAPIC_REG_EOI                   = 0xB0, // W
    LAPIC_REG_RRD                   = 0xC0, // R
    LAPIC_REG_LOGICAL_DESTINATION   = 0xD0, // RW
    LAPIC_REG_DESTINATION_FORMAT    = 0xE0, // RW
    LAPIC_REG_SPURIOUS_VECTOR       = 0xF0, // RW
    LAPIC_REG_ISR                   = 0x100, // R
    LAPIC_REG_TMR                   = 0x180, // R
    LAPIC_REG_IRR                   = 0x200, // R
    LAPIC_REG_ERR_STATUS            = 0x280, // R
    LAPIC_REG_LVT_CMCI              = 0x2F0, // RW
    LAPIC_REG_ICR                   = 0x300, // RW
    LAPIC_REG_LVT_TIMER             = 0x320, // RW
    LAPIC_REG_LVT_THERMAL           = 0x330, // RW
    LAPIC_REG_LVT_PERFORMANCE       = 0x340, // RW
    LAPIC_REG_LVT_LINT0             = 0x350, // RW
    LAPIC_REG_LVT_LINT1             = 0x360, // RW
    LAPIC_REG_LVT_ERROR             = 0x370, // RW
    LAPIC_REG_TIMER_INITIAL_COUNT   = 0x380, // RW
    LAPIC_REG_TIMER_CURRENT_COUNT   = 0x390, // R
    LAPIC_REG_TIMER_DIVIDE_CONFIG   = 0x3E0, // RW
};

#define LVT_DISABLED        (1ULL << 16U)
#define LAPIC_TIMER_HZ      100U

#define LAPIC_READ_REG(reg) (*(volatile uint32_t *)((uint8_t *)g_lapic_base + (reg)))
#define LAPIC_WRITE_REG(reg, val) (*(volatile uint32_t *)((uint8_t *)g_lapic_base + (reg)) = (val))

void lvt_set_entry(int reg, uint8_t vector);
void lapic_timer_init();
void lapic_eoi();
void lapic_init(const madt_header_t *madt);