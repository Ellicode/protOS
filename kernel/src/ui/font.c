/**
 * @file        font.c
 * @brief       Font rendering utilities source file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

#include "ui/vfx.h"
#include "ui/font.h"

void draw_char(
    framebuffer_t *fb,
    char c,
    uint32_t x, uint32_t y,
    uint32_t fg, uint32_t bg
) {
   if (c < ASCII_OFFSET) { return; }

    for (int row = 0; row < FONT_HEIGHT; row++) {
        uint8_t row_data = g_console_font[c - ASCII_OFFSET][row];
        for (int col = 0; col < FONT_WIDTH; col++) {
            put_pixel(fb, x + col, y + row, (row_data & (1U << ((FONT_WIDTH - 1U) - col))) ? fg : bg); 
        }
    }
}