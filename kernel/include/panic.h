/**
 * @file        panic.h
 * @brief       Kernel panic header file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

#pragma once
#include "interrupts/isr.h"

extern const char *panic_messages[];

void panic_handler(const char *message, isr_frame_t *frame);