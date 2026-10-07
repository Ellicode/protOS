/**
 * @file        boot.h
 * @brief       Main boot sequence header file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

#pragma once
#include <stdint.h>
#include "types.h"

#define MAX_MEMMAP_ENTRIES  64
#define MAX_MODULES         8

typedef struct framebuffer_t {
    void        *address;
    uint64_t    width;
    uint64_t    height;
    uint64_t    pitch;
    uint16_t    bpp;    
    uint8_t     red_mask_size;
    uint8_t     red_mask_shift;
    uint8_t     green_mask_size;
    uint8_t     green_mask_shift;
    uint8_t     blue_mask_size;
    uint8_t     blue_mask_shift;
} framebuffer_t;

typedef struct memmap_entry_t {
    uint64_t base;
    uint64_t length;
    uint64_t type;
} memmap_entry_t;

typedef struct boot_module_t {
    void    *address;
    off_t   size;
    char    path[100];
} boot_module_t;

typedef struct boot_info_t {
    int             is_defined;
    char            *protocol;

    uint64_t        hhdm;
    framebuffer_t   framebuffer;

    memmap_entry_t  memmap[MAX_MEMMAP_ENTRIES];
    uint64_t        memmap_entry_count;

    boot_module_t   modules[MAX_MODULES];
    uint64_t        modules_count;

    void            *kernel_phys;
    void            *kernel_virt;
} boot_info_t;

typedef enum {
    BOOT_MEM_USABLE,
    BOOT_MEM_RESERVED,
    BOOT_MEM_ACPI_RECLAIMABLE,
    BOOT_MEM_ACPI_NVS,
    BOOT_MEM_BAD,
    BOOT_MEM_BOOTLOADER_RECLAIMABLE,
    BOOT_MEM_KERNEL_AND_MODULES,
    BOOT_MEM_FRAMEBUFFER,
    BOOT_MEM_RESERVED_MAPPED,
} boot_mem_type_t;

extern boot_info_t g_boot_info;