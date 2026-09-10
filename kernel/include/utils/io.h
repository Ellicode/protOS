/**
 * @file        io.h
 * @brief       Input/output utilities header file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

#pragma once
#include <stdint.h>

typedef uint16_t port_t;

// Send a single byte of data to a specific hardware I/O port
static inline void outb(port_t port, uint8_t value) {
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

// Reads a single byte from a specific hardware I/O port
static inline uint8_t inb(port_t port) {
    unsigned char ret;
    __asm__ volatile ("inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port));
    return ret;
}

// Send a 32 bit integer of data to a specific hardware I/O port
static inline void outl(port_t port, uint32_t value) {
    __asm__ volatile("outl %0, %w1" : : "a"(value), "Nd"(port));
}


// Reads a 32 bit integer from a specific hardware I/O port
static inline uint32_t inl(port_t port) {
    uint32_t ret;
    __asm__ volatile ("inl %w1, %0"
                   : "=a"(ret)
                   : "Nd"(port));
    return ret;
}