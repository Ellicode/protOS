/**
 * @file        gdt.c
 * @brief       Global Descriptor Table setup source file
 * @note        https://osdev.wiki/wiki/Global_Descriptor_Table
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

#include "debug/logger.h"
#include "dt/gdt.h"
#include "mem/pmm.h"

tss_entry_t g_tss __attribute__((aligned(PAGE_SIZE))) = {0};

static gdt_entry_t gdt_generate_descriptor (
    uint64_t base,
    uint64_t limit,
    uint8_t access_byte,
    uint8_t flags
) {
    gdt_entry_t descriptor  = (gdt_entry_t) { 0 };

    descriptor.base_low     = base & 0xFFFFU;
    descriptor.base_mid     = (base >> 16U) & 0xFFU;
    descriptor.base_high    = (base >> 24U) & 0xFFU;

    descriptor.limit_low    = limit & 0xFFFFU;
    descriptor.limit_high   = (limit >> 16U) & 0x0FU;

    /**
     * ACCESS BYTE STRUCTURE
     * ╭───┬───────┬───┬───┬────┬────┬───╮
     * │ 7 │ 6   5 │ 4 │ 3 │ 2  │ 1  │ 0 │
     * ├───┼───────┼───┼───┼────┼────┼───┤
     * │ P │  DPL  │ S │ E │ DC │ RW │ A │
     * ╰───┴───────┴───┴───┴────┴────┴───╯
     * P:   Present
     * DPL: Descriptor privilege level (0 = kernel, 3 = usermode)
     * S:   Descriptor type (0 = system, 1 = code/data segment)
     * E:   Executable
     * DC:  Direction/Confirming bit
     * RW:  Readable/Writable
     *          - Code segments: (1 = readable)
     *          - Data segments: (1 = writable)
     * A:   Accessed
     */
    descriptor.access_byte  = access_byte;

    /**
     * FLAGS STRUCTURE
     * ╭───┬────┬───┬───╮
     * │ 3 │ 2  │ 1 │ 0 │
     * ├───┼────┼───┼───┤
     * │ G │ DB │ L │ . │
     * ╰───┴────┴───┴───╯
     * G:   Granularity (0 = byte, 1 = page)
     * DB:  Size
     * L:   Long mode
     */
    descriptor.flags        = flags;

    return descriptor;
}

static inline void lgdt(dtr_t gdtr) {
    __asm__ volatile (
        "lgdt %0"
        :
        : "m"(gdtr)
        : "memory"
    );
}

void gdt_init() {
    static gdt_entry_t  gdt[7];
    static dtr_t        gdtr;

    gdt[0] = gdt_generate_descriptor(0, 0, 0b00000000, 0b0000);
    gdt[1] = gdt_generate_descriptor(GDT_ENTRY_BASE, GDT_ENTRY_LIMIT, 0b10011011, 0b1010); // Kernel code segment
    gdt[2] = gdt_generate_descriptor(GDT_ENTRY_BASE, GDT_ENTRY_LIMIT, 0b10010011, 0b1010); // Kernel data segment
    gdt[3] = gdt_generate_descriptor(GDT_ENTRY_BASE, GDT_ENTRY_LIMIT, 0b11111011, 0b1010); // Kernel code segment
    gdt[4] = gdt_generate_descriptor(GDT_ENTRY_BASE, GDT_ENTRY_LIMIT, 0b11110011, 0b1010); // Kernel data segment

    g_tss.iopb = sizeof(tss_entry_t);

    uint64_t addr = (uint64_t)&g_tss;
    uint64_t lower  = addr & 0xFFFFFFFF;
    uint64_t higher = addr >> 32U;

    gdt[5] = gdt_generate_descriptor(lower, sizeof(g_tss)-1, 0b10001001, 0b0000); // Task state segment

    uint64_t *gdt_high_tss = (uint64_t*)&gdt[6];
    *gdt_high_tss = higher;

    gdtr.base = (uint64_t)gdt;
    gdtr.limit = sizeof(gdt) - 1;
    kdebug("GDTR (base=0x%x, limit=0x%x)", gdtr.base, gdtr.limit);

    lgdt(gdtr);
    gdt_reload_segments(GDT_OFFSET_KERNEL_CODE, GDT_OFFSET_KERNEL_DATA, GDT_OFFSET_TSS);
}