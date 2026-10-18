/**
 * @file        msr.h
 * @brief       Model Specific Register utilities header file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

#pragma once
#include "panic.h"
#include <stdint.h>
#include <cpuid.h>

typedef uint32_t msr_t;

#define CPUID_FLAG_MSR (1ULL << 5U)

// Checks compatibility with the MSR feature
static inline void msr_check() {
    uint32_t unused, edx;
    __get_cpuid(1, &unused, &unused, &unused, &edx);
    if (!(edx & CPUID_FLAG_MSR)) {
        panic("CPU flag \"MSR\" unsupported");
    }
}

static inline uint64_t rdmsr(msr_t msr) {
    uint32_t lo, hi;
    __asm__ volatile("rdmsr" : "=a"(lo), "=d"(hi) : "c"(msr));
    return ((uint64_t)hi << 32U) | lo;
}

static inline void wrmsr(msr_t msr, uint64_t value) {
    uint32_t lo = (uint32_t)(value & 0xFFFFFFFF);
    uint32_t hi = (uint32_t)(value >> 32U);
    __asm__ volatile("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}