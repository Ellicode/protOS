/**
 * @file        panic.c
 * @brief       Kernel panic source file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

#include "dt/idt.h"
#include "debug/logger.h" 
#include "panic.h"
#include "std/string.h"

const char *panic_messages[] = {
    [EXC_DIVISION_ERROR]        = "DIVISION ERROR",
    [EXC_DEBUG]                 = "DEBUG EXCEPTION",
    [EXC_NMI]                   = "NON MASKABLE INTERRUPT",
    [EXC_BREAKPOINT]            = "BREAKPOINT",
    [EXC_OVERFLOW]              = "OVERFLOW",
    [EXC_BOUND]                 = "BOUND RANGE EXCEEDED",
    [EXC_INVALID_OPCODE]        = "INVALID OPCODE",
    [EXC_DEVICE_UNAVAILABLE]    = "DEVICE UNAVAILABLE",
    [EXC_DOUBLE_FAULT]          = "DOUBLE FAULT",
    [EXC_INVALID_TSS]           = "INVALID TSS",
    [EXC_GP_FAULT]              = "GENERAL PROTECTION",
    [EXC_PAGE_FAULT]            = "PAGE FAULT",
    [EXC_X87_FLOATING_POINT]    = "x87 FPU FLOATING POINT EXCEPTION",
    [EXC_SIMD_FLOATING_POINT]   = "SIMD FLOATING POINT EXCEPTION",
};

void panic_handler(const char *message, isr_frame_t *frame) {
    char extra[255] = "";
    if (frame->vector == EXC_PAGE_FAULT) {
        snprintf(extra, 255, "(user=%d, write=%d, present=%d)", 
            frame->error_code & (1ULL >> 2U),
            frame->error_code & (1ULL >> 1U),
            frame->error_code & (1ULL >> 0U)
        );
    }

    kprintf("\e[31m!!! KERNEL PANIC !!!\n");
    kprintf("reason: \"%s\" at 0x%x %s\n", message, frame->rip, extra);
    kprintf("registers:\n");
    kprintf("rax: 0x%-16x rbx: 0x%-16x rcx: 0x%-16x rdx: 0x%-16x\n", frame->rax, frame->rbx, frame->rcx, frame->rdx);
    kprintf("r8:  0x%-16x r9:  0x%-16x r10: 0x%-16x r11: 0x%-16x\n", frame->r8, frame->r9, frame->r10, frame->r11);
    kprintf("r12: 0x%-16x r13: 0x%-16x r14: 0x%-16x r15: 0x%-16x\n", frame->r12, frame->r13, frame->r14, frame->r15);
    kprintf("rdi: 0x%-16x rsi: 0x%-16x rbp: 0x%-16x\n", frame->rdi, frame->rsi, frame->rbp);

    uint64_t cr0, cr2, cr3, cr4, cr8;
    __asm__ volatile(
        "mov %%cr0, %0\n\t"
        "mov %%cr2, %1\n\t"
        "mov %%cr3, %2\n\t"
        "mov %%cr4, %3\n\t"
        "mov %%cr8, %4\n\t"
        : "=r"(cr0), "=r"(cr2), "=r"(cr3), "=r"(cr4), "=r"(cr8)
    );

    kprintf("control registers:\n");
    kprintf("cr0: 0x%-16x cr2: 0x%-16x cr3: 0x%-16x cr4: 0x%-16x cr8: 0x%-16x\n", cr0, cr2, cr3, cr4, cr8);
    kprintf("\e[0m");

    while (1) {
        __asm__ ("hlt");
    }
}