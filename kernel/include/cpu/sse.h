/**
 * @file        sse.h
 * @brief       SSE Initialization header file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

#pragma once

#define CPUID_FEAT_FPU  (1 << 0)
#define CPUID_FEAT_SSE  (1 << 25)
#define CPUID_FEAT_SSE2 (1 << 26)

extern int g_sse_enabled;

void fpu_sse_init(void);