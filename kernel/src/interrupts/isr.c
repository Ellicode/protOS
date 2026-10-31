/**
 * @file        isr.c
 * @brief       Interrupt service routine source file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

#include "dt/idt.h"
#include "interrupts/isr.h"
#include "interrupts/lapic.h"
#include "drivers/pit.h"
#include "debug/logger.h"
#include "panic.h"

void isr_handler(isr_frame_t *frame) {
    uint64_t vec = frame->vector;
    switch (vec) {
        case IRQ_LAPIC_TIMER:
            kdebug("tick!");
            g_lapic_timer_ticks++;
            break;

        case IRQ_PIT_TIMER:
            g_pit_ticks++;
            break;

        default:
            if (vec < EXCEPTION_COUNT) {
                panic_handler(panic_messages[vec], frame);
            } else {
                kdebug("Uncaught interrupt recieved (vec: %d)", frame->vector);
            }
            break;
    }

    if (vec >= EXCEPTION_COUNT && vec <= EXCEPTION_COUNT + IRQ_COUNT) {
        lapic_eoi();
    }
}