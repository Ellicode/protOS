/**
 * @file        vfx.c
 * @brief       Visual effects utilities source file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

#include <stddef.h>
#include "ui/vfx.h"

// Gets the color value compatible with the current framebuffer format
uint32_t get_direct_color(uint32_t color) {
    uint32_t direct_color = 0;
    direct_color =
            ((color >> 24U) >>
            (8U - g_boot_info.framebuffer.red_mask_size)) <<
            g_boot_info.framebuffer.red_mask_shift;
    direct_color |=
            ((color >> 16U & 0xFFU) >>
            (8U - g_boot_info.framebuffer.green_mask_size)) <<
            g_boot_info.framebuffer.green_mask_shift;
    direct_color |=
            ((color >> 8U & 0xFFU) >>
            (8U - g_boot_info.framebuffer.blue_mask_size)) <<
            g_boot_info.framebuffer.blue_mask_shift;

    return direct_color;
}

// Draws a singular pixel to the framebuffer.
void put_pixel(
    uint32_t x, uint32_t y, 
    uint32_t color
) {
    if (g_boot_info.framebuffer.address == NULL) { return; }
    if (x >= g_boot_info.framebuffer.width || y >= g_boot_info.framebuffer.height) { return; }

    void *fb_ptr = g_boot_info.framebuffer.address + (y * g_boot_info.framebuffer.pitch) + (x * g_boot_info.framebuffer.bpp / 8);
    
    switch (g_boot_info.framebuffer.bpp) {
        case 32:
            *(uint32_t *)fb_ptr = get_direct_color(color);
            break;
        case 16:
            *(uint16_t *)fb_ptr = get_direct_color(color);
            break;
        case 8:
            *(uint8_t *)fb_ptr = get_direct_color(color);
            break;
        default:
            break;
    }
}

void invert_rect(
    uint32_t x, uint32_t y,
    uint32_t w, uint32_t h
) {
    uint8_t *fb_base = (uint8_t *)g_boot_info.framebuffer.address;
    uint32_t pitch = g_boot_info.framebuffer.pitch;
    uint32_t bytes_per_pixel = g_boot_info.framebuffer.bpp / 8;

    for (uint32_t row = y; row < y + h; row++) {
        
        uint8_t *row_ptr = fb_base + (row * pitch);
        uint32_t *pixel = (uint32_t *)(row_ptr + (x * bytes_per_pixel));

        for (uint32_t col = 0; col < w; col++) {
            *pixel ^= 0x00FFFFFFU;

            pixel++; 
        }
    }
}