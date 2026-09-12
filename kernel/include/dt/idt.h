/**
 * @file        idt.h
 * @brief       Interrupt Descriptor Table header file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

#pragma once
#include <stdint.h>

#define EXCEPTION_COUNT 32

enum {
    EXC_DIVISION_ERROR          = 0,
    EXC_DEBUG                   = 1,
    EXC_NMI                     = 2,
    EXC_BREAKPOINT              = 3,
    EXC_OVERFLOW                = 4,
    EXC_BOUND                   = 5,
    EXC_INVALID_OPCODE          = 6,
    EXC_DEVICE_UNAVAILABLE      = 7,
    EXC_DOUBLE_FAULT            = 8,
    EXC_INVALID_TSS             = 10,
    EXC_GP_FAULT                = 13,
    EXC_PAGE_FAULT              = 14,
    EXC_X87_FLOATING_POINT      = 16,
    EXC_SIMD_FLOATING_POINT     = 19,
};

typedef struct idt_entry_t {
	uint16_t    isr_low;
	uint16_t    kernel_cs;
	uint8_t	    ist;
	uint8_t     attributes;
	uint16_t    isr_mid;
	uint32_t    isr_high;
	uint32_t    reserved;
} __attribute__((packed)) idt_entry_t;

extern void isr_call_0();   // Division error
extern void isr_call_1();   // Debug exception
extern void isr_call_2();   // Non maskable interrupt
extern void isr_call_3();   // Breakpoint exception
extern void isr_call_4();   // Overflow exeption
extern void isr_call_5();   // Bound exception
extern void isr_call_6();   // Invalid opcode
extern void isr_call_7();   // Device unavailable
extern void isr_call_8();   // Double fault
extern void isr_call_10();  // Invalid TSS
extern void isr_call_13();  // General Protection fault (i hate you)
extern void isr_call_14();  // Page fault
extern void isr_call_16();  // x87 floating point error
extern void isr_call_19();  // SIMD floating point error

void idt_init();

int interrupts_enabled();
void enable_interrupts();
void disable_interrupts();
void restore_interrupts(int irqs);