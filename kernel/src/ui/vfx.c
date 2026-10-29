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
uint32_t get_direct_color(framebuffer_t *fb, uint32_t color) {
    uint32_t direct_color = 0;
    direct_color =
            ((color >> 24U) >>
            (8U - fb->red_mask_size)) <<
            fb->red_mask_shift;
    direct_color |=
            ((color >> 16U & 0xFFU) >>
            (8U - fb->green_mask_size)) <<
            fb->green_mask_shift;
    direct_color |=
            ((color >> 8U & 0xFFU) >>
            (8U - fb->blue_mask_size)) <<
            fb->blue_mask_shift;

    return direct_color;
}

// Draws a singular pixel to the framebuffer.
void put_pixel(
    framebuffer_t *fb,
    uint32_t x, uint32_t y, 
    uint32_t color
) {
    if (fb->address == NULL) { return; }
    if (x >= fb->width || y >= fb->height) { return; }

    void *fb_ptr = fb->address + (y * fb->pitch) + (x * fb->bpp / 8);
    
    switch (fb->bpp) {
        case 32:
            *(uint32_t *)fb_ptr = get_direct_color(fb, color);
            break;
        case 16:
            *(uint16_t *)fb_ptr = get_direct_color(fb, color);
            break;
        case 8:
            *(uint8_t *)fb_ptr = get_direct_color(fb, color);
            break;
        default:
            break;
    }
}

void invert_rect(
    framebuffer_t *fb,
    uint32_t x, uint32_t y,
    uint32_t w, uint32_t h
) {
    uint8_t *fb_base = (uint8_t *)fb->address;
    uint32_t pitch = fb->pitch;
    uint32_t bytes_per_pixel = fb->bpp / 8;

    for (uint32_t row = y; row < y + h; row++) {
        
        uint8_t *row_ptr = fb_base + (row * pitch);
        uint32_t *pixel = (uint32_t *)(row_ptr + (x * bytes_per_pixel));

        for (uint32_t col = 0; col < w; col++) {
            *pixel ^= 0x00FFFFFFU;

            pixel++; 
        }
    }
}