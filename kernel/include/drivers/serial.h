/**
 * @file        serial.h
 * @brief       Serial utilities header file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

#pragma once

// Serial output port (COM1)
#define SERIAL_PORT 0x3F8

int serial_init();
void serial_write(unsigned char c);
unsigned char serial_read();