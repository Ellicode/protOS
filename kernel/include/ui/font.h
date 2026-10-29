/**
 * @file        font.h
 * @brief       Font rendering utilities header file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

#pragma once
#include <stdint.h>

#define ASCII_OFFSET    32
#define FONT_WIDTH      8
#define FONT_HEIGHT     16

extern const unsigned char g_console_font[][16];

void draw_char(
    framebuffer_t *fb,
    char c,
    uint32_t x, uint32_t y,
    uint32_t fg, uint32_t bg
);