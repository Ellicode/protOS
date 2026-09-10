/**
 * @file        isr.c
 * @brief       Interrupt service routine source file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

#include "dt/idt.h"
#include "interrupts/isr.h"
#include "debug/logger.h"
#include "panic.h"

void isr_handler(isr_frame_t *frame) {
    // NOTE: Move to enum after there's more possible vectors
    uint64_t vec = frame->vector;
    if (vec < EXCEPTION_COUNT) {
        panic_handler(panic_messages[frame->vector], frame);
    }
}