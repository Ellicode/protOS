/**
 * @file        console.c
 * @brief       Console rendering source file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

#include <stddef.h>
#include "boot.h"
#include "ui/font.h"
#include "ui/console.h"
#include "ui/vfx.h"
#include "drivers/serial.h"
#include "std/string.h"

const uint32_t color_palette[] = {
    0x00000000,
    0xCC000000,
    0x00CC0000,
    0xCC770000,
    0x0000CC00,
    0xCC00CC00,
    0x00CCCC00,
    0xCCCCCC00,
};

const uint32_t color_palette_bright[] = {
    0x55555500,
    0xFF555500,
    0x55FF5500,
    0xFFFF5500,
    0x5555FF00,
    0xFF55FF00,
    0x55FFFF00,
    0xFFFFFF00,
};

uint64_t cursor_row     = 0; 
uint64_t cursor_col     = 0;
uint32_t current_fg     = color_palette[7];
uint32_t current_bg     = color_palette[0];
int cursor_visibility   = 0;
int bright              = 0;

void scroll_y(int cols) {
    if (cols == 0) { return; }

    uint8_t *fb_base = (uint8_t *)g_boot_info.framebuffer.address;
    uint32_t fb_width = g_boot_info.framebuffer.width;
    uint32_t fb_height = g_boot_info.framebuffer.height;

    size_t row_bytes = fb_width * sizeof(uint32_t); 
    
    size_t abs_cols = (cols > 0) ? cols : -cols;
    size_t pixel_rows = abs_cols * FONT_HEIGHT;

    if (pixel_rows >= fb_height) {
        memset(fb_base, 0, fb_height * row_bytes);
        return;
    }

    size_t offset_bytes = pixel_rows * row_bytes;
    size_t bytes_to_move = (fb_height - pixel_rows) * row_bytes;

    if (cols > 0) {
        memmove(
            fb_base, 
            fb_base + offset_bytes, 
            bytes_to_move
        );
        memset(
            fb_base + bytes_to_move, 
            0, 
            offset_bytes
        );
    } else {
        memmove(
            fb_base + offset_bytes, 
            fb_base, 
            bytes_to_move
        );
        memset(
            fb_base, 
            0, 
            offset_bytes
        );
    }
}

void render_cursor() {
    invert_rect(cursor_col * FONT_WIDTH, cursor_row * FONT_HEIGHT, FONT_WIDTH, FONT_HEIGHT);
}

void print_char(char c) {
    uint64_t max_cols = g_boot_info.framebuffer.width / FONT_WIDTH;
    uint64_t max_rows = g_boot_info.framebuffer.height / FONT_HEIGHT;

    if (cursor_visibility == 1) { render_cursor(); }

    switch (c) {
        case '\n':
            cursor_col = 0;
            cursor_row++;
            break;
        case '\b':
            if (cursor_col > 0) {
                cursor_col--;
            }
            break;
        case '\t':
            cursor_col = ((cursor_col / TAB_WIDTH) + 1) * TAB_WIDTH;            
            if (cursor_col >= max_cols) {
                cursor_col = 0;
                cursor_row++;
            }
            break;
        default:
            draw_char(c, cursor_col * FONT_WIDTH, cursor_row * FONT_HEIGHT, current_fg, current_bg);
            cursor_col++;
            if (cursor_col >= max_cols) {
                cursor_col = 0;
                cursor_row++;
            }
            break;
    }

    if (cursor_row >= max_rows) {
        scroll_y(1);
        cursor_row = max_rows - 1;
    }

    if (cursor_visibility == 1) { render_cursor(); }
}

void set_color(uint32_t fg, uint32_t bg) {
    current_fg = fg;
    current_bg = bg;
}

void show_cursor() {
    if (cursor_visibility == 0) { render_cursor(); }
    cursor_visibility = 1;
}

void hide_cursor() {
    if (cursor_visibility == 1) { render_cursor(); }
    cursor_visibility = 0;
}

void toggle_cursor() {
    if (cursor_visibility == 0) { show_cursor(); }
    else { hide_cursor(); }
}

void set_cursor(uint32_t x, uint32_t y) {
    uint64_t max_cols = g_boot_info.framebuffer.width / FONT_WIDTH;
    uint64_t max_rows = g_boot_info.framebuffer.height / FONT_HEIGHT;

    if (x > max_cols || y > max_rows) { return; }

    if (cursor_visibility == 1) { render_cursor(); }
    cursor_col = x;
    cursor_row = y;
    if (cursor_visibility == 1) { render_cursor(); }
}

void print(char *str) {
    size_t len = strlen(str);
    size_t i = 0;
    while (i < len) {
        serial_write(str[i]);
        if (str[i] == '\e' && str[i + 1] == '[') {
            volatile int args[4] = { 0 };
            int arg_len = 0;
            i += 2; // Skip \e and [
            serial_write('\e');
            serial_write('[');
            while (i < len) {
                serial_write(str[i]);
                if (str[i] == ';') {
                    if (arg_len < 15) { arg_len++; }
                } else if (str[i] >= '0' && str[i] <= '9') {
                    args[arg_len] = (args[arg_len] * 10) + (str[i] - '0');
                } else if (str[i] == 'm') {
                    for (int j = 0; j <= arg_len; j++) {
                        int arg = args[j];
                        if (arg == 0) { // Reset escape code
                            bright = 0;
                            set_color(color_palette[7], color_palette[0]);
                        } else if (arg == 1) {
                            bright = 1;
                        } else if (arg >= 90 && arg <= 97) {
                            current_fg = color_palette_bright[arg - 90];
                        } else if (arg >= 100 && arg <= 107) {
                            current_bg = color_palette_bright[arg - 100];
                        } else if (arg >= 30 && arg <= 37) {
                            current_fg = color_palette[arg - 30];
                        } else if (arg >= 40 && arg <= 47) {
                            current_bg = color_palette[arg - 40];
                        }
                    }
                    i++;
                    break;
                } else if (str[i] == 'A') { // Move cursor up
                    cursor_row -= args[0] == 0 ? 1 : args[0];
                    set_cursor(cursor_row, cursor_col);
                    i++;
                    break;
                } else if (str[i] == 'B') { // Move cursor down
                    cursor_row += args[0] == 0 ? 1 : args[0];
                    set_cursor(cursor_row, cursor_col);
                    i++;
                    break;
                } else if (str[i] == 'C') { // Move cursor right
                    cursor_col += args[0] == 0 ? 1 : args[0];
                    set_cursor(cursor_row, cursor_col);
                    i++;
                    break;
                } else if (str[i] == 'D') { // Move cursor left
                    cursor_col -= args[0] == 0 ? 1 : args[0];
                    set_cursor(cursor_row, cursor_col);
                    i++;
                    break;
                } else if (str[i] == 'E') { // Move cursor to the next line
                    cursor_row += args[0] == 0 ? 1 : args[0];
                    set_cursor(cursor_row, 0);
                    i++;
                    break;
                } else if (str[i] == 'F') { // Move cursor to the previous line
                    cursor_row -= args[0] == 0 ? 1 : args[0];
                    set_cursor(cursor_row, 0);
                    i++;
                    break;
                } else if (str[i] == 'H') { // Set cursor position
                    int x = args[0];
                    int y = args[1];
                    set_cursor(x, y);
                    i++;
                    break;
                } else if (str[i] == 'S') { // Scroll up
                    int cols = args[0] == 0 ? 1 : args[0];
                    scroll_y(cols);
                } else if (str[i] == 'T') { // Scroll down
                    int cols = args[0] == 0 ? 1 : args[0];
                    scroll_y(-cols);
                } else {
                    i++;
                    break;
                }
                i++;
            }
        } else {
            print_char(str[i]);
            i++;
        }
    }
}

int kprintf(const char *restrict format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[1024];
    int ret = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    print(buffer);
    return ret;
}