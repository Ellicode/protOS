/**
 * @file        align.h
 * @brief       Alignment and rounding utilities header file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

#pragma once
#include <stdint.h>
#include "mem/pmm.h"

#define ALIGN_UP(x, align) ((((uint64_t) (x)) + ((align) - 1)) & ~((uint64_t) ((align) - 1)))
#define ALIGN_DOWN(x, align) (((uint64_t) (x)) & ~((uint64_t) ((align) - 1)))

#define ROUND_UP(x, round)(((((uint64_t)(x) + (uint64_t)(round) - 1) / (uint64_t)(round) ) * (uint64_t)(round)))

#define PAGE_ALIGN(x) ALIGN_UP(x, PAGE_SIZE)
#define PAGE_ROUND(x) ROUND_UP(x, PAGE_SIZE)

#define CLAMP(val, min, max) ((val) < (min) ? (min) : ((val) > (max) ? (max) : (val)))
#define MIN(a, b) ((a) < (b) ? (a) : (b))