/**
 * @file        boot.h
 * @brief       Main boot sequence header file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

#pragma once
#include <stdint.h>

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

typedef struct boot_info_t {
    int             is_defined;
    char            protocol[16];

    uint64_t        hhdm;
    framebuffer_t   framebuffer;

    memmap_entry_t  **memmap;
    uint64_t        memmap_entry_count;

    void            *kernel_phys;
    void            *kernel_virt;
} boot_info_t;

extern boot_info_t g_boot_info;