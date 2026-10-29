/**
 * @file        console.h
 * @brief       Console rendering header file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

#pragma once

#define TAB_WIDTH       8UL

void show_cursor();
void hide_cursor();
void print_char(char c);
void print(char *str);
int kprintf(const char *restrict format, ...);
void fbcon_init();