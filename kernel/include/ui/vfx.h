/**
 * @file        vfx.h
 * @brief       Visual effects utilities header file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

#pragma once
#include "boot.h"

uint32_t get_direct_color(framebuffer_t *fb, uint32_t color);

void put_pixel(
    framebuffer_t *fb,
    uint32_t x, uint32_t y, 
    uint32_t color
);

void invert_rect(
    framebuffer_t *fb,
    uint32_t x, uint32_t y,
    uint32_t w, uint32_t h
);