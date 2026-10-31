/**
 * @file        pit.c
 * @brief       Legacy PIT driver used to calibrate the LAPIC timer
 *
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
*/

#include "drivers/pit.h"
#include "debug/logger.h"
#include "dt/idt.h"
#include "interrupts/ioapic.h"
#include "interrupts/lapic.h"
#include "utils/io.h"

#define PIT_CHANNEL_0           0x40U
#define PIT_COMMAND             0x43U
#define PIT_INPUT_FREQUENCY     1193182U
#define PIT_CALIBRATION_HZ      100U
#define PIT_CALIBRATION_TICKS   10U
#define PIT_IRQ_VECTOR          33U
#define PIT_IRQ                 0U
#define PIT_TIMEOUT_ITERATIONS  1000000000ULL

uint32_t g_pit_ticks;

uint32_t pit_calibrate(void) {
    int interrupts_were_enabled = interrupts_enabled();
    disable_interrupts();

    if (!ioapic_set_irq_route(PIT_IRQ, PIT_IRQ_VECTOR, 1)) {
        kerror("Could not route PIT IRQ through IOAPIC");
        restore_interrupts(interrupts_were_enabled);
        return 0;
    }

    uint16_t divisor = (uint16_t)(PIT_INPUT_FREQUENCY / PIT_CALIBRATION_HZ);
    g_pit_ticks = 0;

    LAPIC_WRITE_REG(LAPIC_REG_TIMER_DIVIDE_CONFIG, 3);
    LAPIC_WRITE_REG(LAPIC_REG_LVT_TIMER, LVT_DISABLED | 32U);
    LAPIC_WRITE_REG(LAPIC_REG_TIMER_INITIAL_COUNT, 0xFFFFFFFFU);

    outb(PIT_COMMAND, 0x34U);
    outb(PIT_CHANNEL_0, (uint8_t)(divisor & 0xFFU));
    outb(PIT_CHANNEL_0, (uint8_t)(divisor >> 8U));

    if (!ioapic_set_irq_route(PIT_IRQ, PIT_IRQ_VECTOR, 0)) {
        LAPIC_WRITE_REG(LAPIC_REG_TIMER_INITIAL_COUNT, 0);
        ioapic_set_irq_route(PIT_IRQ, 32U, 1);
        kerror("Could not unmask PIT IRQ through IOAPIC");
        restore_interrupts(interrupts_were_enabled);
        return 0;
    }

    enable_interrupts();
    uint64_t timeout = PIT_TIMEOUT_ITERATIONS;
    while (g_pit_ticks < PIT_CALIBRATION_TICKS && timeout > 0) {
        __asm__ volatile("pause");
        timeout--;
    }
    disable_interrupts();

    uint32_t current_count = LAPIC_READ_REG(LAPIC_REG_TIMER_CURRENT_COUNT);
    uint32_t elapsed_counts = 0xFFFFFFFFU - current_count;
    LAPIC_WRITE_REG(LAPIC_REG_TIMER_INITIAL_COUNT, 0);
    ioapic_set_irq_route(PIT_IRQ, 32U, 1);

    uint32_t ticks = g_pit_ticks;
    restore_interrupts(interrupts_were_enabled);

    if (ticks < PIT_CALIBRATION_TICKS || elapsed_counts == 0) {
        kerror("PIT/LAPIC timer calibration timed out");
        return 0;
    }

    uint64_t denominator = (uint64_t)divisor * PIT_CALIBRATION_TICKS;
    uint64_t timer_frequency = ((uint64_t)elapsed_counts * PIT_INPUT_FREQUENCY) / denominator;
    if (timer_frequency == 0 || timer_frequency > UINT32_MAX) {
        kerror("Invalid LAPIC timer calibration result");
        return 0;
    }

    kdebug("LAPIC timer frequency: %d Hz", (uint32_t)timer_frequency);
    return (uint32_t)timer_frequency;
}