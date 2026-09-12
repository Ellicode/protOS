/**
 * @file        idt.c
 * @brief       Interrupt Descriptor Table source file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

#include "debug/logger.h"
#include "dt/gdt.h"
#include "dt/idt.h"

void* isr_exception_handlers[EXCEPTION_COUNT] = {
    [EXC_DIVISION_ERROR]        = isr_call_0,
    [EXC_DEBUG]                 = isr_call_1,
    [EXC_NMI]                   = isr_call_2,
    [EXC_BREAKPOINT]            = isr_call_3,
    [EXC_OVERFLOW]              = isr_call_4,
    [EXC_BOUND]                 = isr_call_5,
    [EXC_INVALID_OPCODE]        = isr_call_6,
    [EXC_DEVICE_UNAVAILABLE]    = isr_call_7,
    [EXC_DOUBLE_FAULT]          = isr_call_8,
    [EXC_INVALID_TSS]           = isr_call_10,
    [EXC_GP_FAULT]              = isr_call_13,
    [EXC_PAGE_FAULT]            = isr_call_14,
    [EXC_X87_FLOATING_POINT]    = isr_call_16,
    [EXC_SIMD_FLOATING_POINT]   = isr_call_19,
};

static idt_entry_t idt_generate_descriptor (
    void *isr,
    uint8_t attributes
) {
    idt_entry_t descriptor  = (idt_entry_t) { 0 };

    descriptor.isr_low      = (uint64_t)isr & 0xFFFFU;
    descriptor.isr_mid      = ((uint64_t)isr >> 16U) & 0xFFFFU;
    descriptor.isr_high     = ((uint64_t)isr >> 32U) & 0xFFFFFFFFU;
    descriptor.kernel_cs    = GDT_OFFSET_KERNEL_CODE;
    
    /**
     * ATTRIBUTES STRUCTURE
     * ╭───┬──────┬───┬───────────────╮
     * │ 7 │ 6  5 │ 4 │ 3   2   1   0 │
     * ├───┼──────┼───┼───────────────┤
     * │ P │ DPL  │ . │   GATE TYPE   │
     * ╰───┴──────┴───┴───────────────╯
     * P:   Present
     * DPL: Descriptor privilege level (0 = kernel, 3 = usermode)
     * GT:  Gate type (0b1110 = interrupt, 0b1111 = trap)
     */
    descriptor.attributes   = attributes;

    return descriptor;
}

static inline void lidt(dtr_t idtr) {
    __asm__ volatile (
        "lidt %0"
        :
        : "m"(idtr)
        : "memory"
    );
}

void idt_init() {
    static idt_entry_t  idt[256];
    static dtr_t        idtr;

    for (int i = 0; i < EXCEPTION_COUNT; i++) {
        if (isr_exception_handlers[i] == 0) { continue; }
        idt[i] = idt_generate_descriptor(isr_exception_handlers[i], 0b10001110);
    }

    idtr.base = (uint64_t)idt;
    idtr.limit = sizeof(idt) - 1;
    kdebug("IDTR (base=0x%x, limit=0x%x)", idtr.base, idtr.limit);

    lidt(idtr);
}

int interrupts_enabled() {
    uint64_t rflags;
    __asm__ volatile (
        "pushfq\n\t"
        "pop %0" : 
        "=r" (rflags)
    );
    return (rflags >> 9U) & 1U;
}

void enable_interrupts() {
    __asm__ volatile ("sti");
}

void disable_interrupts() {
    __asm__ volatile ("cli");
}

void restore_interrupts(int irqs) {
    if (irqs) {
        enable_interrupts();
    } else {
        disable_interrupts();
    }
}