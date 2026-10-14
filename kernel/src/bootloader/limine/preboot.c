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

__attribute__((used, section(".limine_requests"))) static volatile struct limine_executable_address_request executable_address_request = {
    .id = LIMINE_EXECUTABLE_ADDRESS_REQUEST_ID,
    .revision = 0,
};

__attribute__((used, section(".limine_requests"))) static volatile struct limine_module_request module_request = {
    .id = LIMINE_MODULE_REQUEST_ID,
    .revision = 0,
};

__attribute__((used, section(".limine_requests"))) static volatile struct limine_rsdp_request rsdp_request = {
    .id = LIMINE_RSDP_REQUEST_ID,
    .revision = 0,
};

static int memmap_entry_types[] = {
    [LIMINE_MEMMAP_USABLE] = BOOT_MEM_USABLE,
    [LIMINE_MEMMAP_RESERVED] = BOOT_MEM_RESERVED,
    [LIMINE_MEMMAP_ACPI_RECLAIMABLE] = BOOT_MEM_ACPI_RECLAIMABLE,
    [LIMINE_MEMMAP_ACPI_NVS] = BOOT_MEM_ACPI_NVS,
    [LIMINE_MEMMAP_BAD_MEMORY] = BOOT_MEM_BAD,
    [LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE] = BOOT_MEM_BOOTLOADER_RECLAIMABLE,
    [LIMINE_MEMMAP_EXECUTABLE_AND_MODULES] = BOOT_MEM_KERNEL_AND_MODULES,
    [LIMINE_MEMMAP_FRAMEBUFFER] = BOOT_MEM_KERNEL_AND_MODULES,
    [LIMINE_MEMMAP_RESERVED_MAPPED] = BOOT_MEM_RESERVED_MAPPED,
};

void limine_preboot() {
    g_boot_info.is_defined = 1;
    g_boot_info.protocol = "LIMINE";

    if (framebuffer_request.response && framebuffer_request.response->framebuffer_count > 0) {
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
    }

    if (memmap_request.response) {
        g_boot_info.memmap_entry_count  = (int)memmap_request.response->entry_count;
        for (uint64_t i = 0; i < g_boot_info.memmap_entry_count; i++) {
            struct limine_memmap_entry *entry = memmap_request.response->entries[i];

            g_boot_info.memmap[i].base      = entry->base;
            g_boot_info.memmap[i].length    = entry->length;
            g_boot_info.memmap[i].type      = memmap_entry_types[entry->type];
        }
    }

    if (module_request.response) {
        g_boot_info.modules_count       = (int)module_request.response->module_count;
        for (uint64_t i = 0; i < g_boot_info.modules_count; i++) {
            struct limine_file *entry = module_request.response->modules[i];

            g_boot_info.modules[i].address  = entry->address;
            g_boot_info.modules[i].size     = (off_t)entry->size;
            strncpy(g_boot_info.modules[i].path, entry->path, 100);
        }
    }

    if (hhdm_request.response) {
        g_boot_info.hhdm                = hhdm_request.response->offset;
    }

    if (executable_address_request.response) {
        g_boot_info.kernel_phys         = (void *)executable_address_request.response->physical_base;
        g_boot_info.kernel_virt         = (void *)executable_address_request.response->virtual_base;
    }

    if (rsdp_request.response) {
        g_boot_info.rsdp                = rsdp_request.response->address;
    }
}