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
#include "mem/pmm.h"
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

    kdebug("Memory map entry count: %d", g_boot_info.memmap_entry_count);
    for (int i = 0; i < g_boot_info.memmap_entry_count; i++) {
        kdebug("(%d)\tbase: %16x size: %16x type: %d", i, g_boot_info.memmap[i]->base, g_boot_info.memmap[i]->length, g_boot_info.memmap[i]->type);
    }

    fpu_sse_init();
    ksuccess("SSE init");

    gdt_init();
    ksuccess("GDT init");

    idt_init();
    ksuccess("IDT init");

    fpmm_init();
    ksuccess("PMM init");
    kinfo("Current PMM model: \"%s\"", g_pmm.type);

    char *buf = palloc();
    strcpy(buf, "hello from freelist");
    kassert("Buffer contents: %s", buf);
    pfree(buf);
    kassert("Buffer freed (size=%d, used=%d)", psize(), pused());

    kinfo("Welcome to \e[36mProtOS\e[0m! Enjoy your stay :3");

    while (1) {
        __asm__("hlt");
    }
}