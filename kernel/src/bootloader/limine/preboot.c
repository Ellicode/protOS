/**
 * @file        preboot.c
 * @brief       Limine-specific pre-boot setup source file.
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

#include "_deps/limine.h"
#include "std/string.h"
#include "boot.h"

__attribute__((used, section(".limine_requests"))) static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0,
};

__attribute__((used, section(".limine_requests"))) static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST_ID,
    .revision = 0,
};

__attribute__((used, section(".limine_requests"))) static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST_ID,
    .revision = 0,
};

void limine_preboot() {
    g_boot_info.is_defined = 1;
    strcpy(g_boot_info.protocol, "LIMINE");

    struct limine_framebuffer *fb = framebuffer_request.response->framebuffers[0];
    g_boot_info.framebuffer = (framebuffer_t) {
        .address            = fb->address,
        .width              = fb->width,
        .height             = fb->height,
        .pitch              = fb->pitch,
        .bpp                = fb->bpp,
        .red_mask_size      = fb->red_mask_size,
        .red_mask_shift     = fb->red_mask_shift,
        .green_mask_size    = fb->green_mask_size,
        .green_mask_shift   = fb->green_mask_shift,
        .blue_mask_size     = fb->blue_mask_size,
        .blue_mask_shift    = fb->blue_mask_shift,
    };

    g_boot_info.memmap              = (memmap_entry_t **)memmap_request.response->entries;
    g_boot_info.memmap_entry_count  = (int)memmap_request.response->entry_count;

    g_boot_info.hhdm = hhdm_request.response->offset;
}