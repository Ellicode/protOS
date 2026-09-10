/**
 * @file        gdt.h
 * @brief       Global Descriptor Table setup header file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

#pragma once
#include <stdint.h>

#define GDT_ENTRY_BASE          0x00000
#define GDT_ENTRY_LIMIT         0xFFFFF

#define GDT_OFFSET_KERNEL_CODE  (8*1) // 8
#define GDT_OFFSET_KERNEL_DATA  (8*2) // 16
#define GDT_OFFSET_USER_CODE    (8*3) // 24
#define GDT_OFFSET_USER_DATA    (8*4) // 32
#define GDT_OFFSET_TSS          (8*5) // 40

typedef struct gdt_entry_t {
    uint16_t        limit_low;
    uint16_t        base_low;
    uint8_t         base_mid;
    uint8_t         access_byte;
    uint8_t         limit_high  : 4;
    uint8_t         flags       : 4;
    uint8_t         base_high;
} __attribute__((packed)) gdt_entry_t;

typedef struct dtr_t {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) dtr_t;

typedef struct tss_entry_t {
    uint32_t reserved0;

    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;

    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;

    uint64_t reserved1;

    uint16_t reserved2;
    uint16_t iopb;
} __attribute__((packed)) tss_entry_t;

extern void gdt_reload_segments(uint16_t cs, uint16_t ds, uint16_t tss);

int gdt_init();