/**
 * @file        boot.c
 * @brief       Main boot sequence source file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

#include "bootloader/limine/preboot.h"
#include "boot.h"
#include "ui/console.h"
#include "debug/logger.h"
#include "dt/gdt.h"
#include "dt/idt.h"
#include "drivers/serial.h"
#include "cpu/sse.h"
#include "mem/freelist.h"
#include "mem/vmm.h"
#include "mem/pmm.h"
#include "mem/heap.h"
#include "std/string.h"

boot_info_t g_boot_info = (boot_info_t) { .is_defined = 0 };

void kboot() {
    limine_preboot();

    // Check if we actually defined our pre-boot structure with a known booting convention.
    if (g_boot_info.is_defined == 0) { 
        return;
    }

    serial_init();
    hide_cursor();

    kinfo("Boot protocol: \"%s\"", g_boot_info.protocol);
    kinfo("Framebuffer information: (width=%d, height=%d, bpp=%d)", 
        g_boot_info.framebuffer.width, 
        g_boot_info.framebuffer.height, 
        g_boot_info.framebuffer.bpp
    );
    kdebug("\e[40m  \e[41m  \e[42m  \e[43m  \e[44m  \e[45m  \e[46m  \e[47m  \e[0m");
    kdebug("\e[100m  \e[101m  \e[102m  \e[103m  \e[104m  \e[105m  \e[106m  \e[107m  \e[0m");

    kdebug("Memory map entry count: %d", g_boot_info.memmap_entry_count);
    kdebug("+--------+--------------------+--------------------+------+");
    kdebug("| Offset | Base               | Size               | Type |");
    kdebug("+--------+--------------------+--------------------+------+");
    for (uint64_t i = 0; i < g_boot_info.memmap_entry_count; i++) {
        kdebug("| %-6d | 0x%-16x | 0x%-16x | %4d |", i, g_boot_info.memmap[i]->base, g_boot_info.memmap[i]->length, g_boot_info.memmap[i]->type);
    }
    kdebug("+--------+--------------------+--------------------+------+");

    fpu_sse_init();
    ksuccess("SSE init");

    gdt_init();
    ksuccess("GDT init");

    idt_init();
    ksuccess("IDT init");

    fpmm_init();
    ksuccess("PMM init");
    kdebug("Current PMM model: \"%s\"", g_pmm.type);

    vmm_init();
    ksuccess("Paging init");

    heap_init();
    ksuccess("Heap init");

    kinfo("Welcome to \e[36mProtOS\e[37m! Enjoy your stay :3");

    while (1) {
        __asm__("hlt");
    }
}