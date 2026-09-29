/**
 * @file        sse.c
 * @brief       SSE Initialization source file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

#include "debug/logger.h"
#include "cpu/sse.h"
#include "panic.h"
#include <cpuid.h>
#include <stdint.h>

int g_sse_enabled = 0;

void fpu_sse_init(void) {
    uint32_t unused, edx;
    __get_cpuid(1, &unused, &unused, &unused, &edx);

    if ((edx & CPUID_FEAT_FPU) && (edx & CPUID_FEAT_SSE) && (edx & CPUID_FEAT_SSE2)) {
        uint64_t cr0;
        uint64_t cr4;

        __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
        cr0 |= 0x22U;
        cr0 &= ~0xCU; 
        __asm__ volatile("mov %0, %%cr0" :: "r"(cr0));

        __asm__ volatile("mov %%cr4, %0" : "=r"(cr4));
        cr4 |= 0x600U; 
        __asm__ volatile("mov %0, %%cr4" :: "r"(cr4));

        __asm__ volatile("fninit" ::: "memory");

        g_sse_enabled = 1;
    } else {
        panic("CPU flag \"FPU\" unsupported");
    }
}
