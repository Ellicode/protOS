/**
 * @file        vmm.h
 * @brief       Virtual Memory Manager header file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
*/

#pragma once
#include <stddef.h>
#include <stdint.h>

extern uint64_t *g_kern_pml4;

#define VMM_PRESENT     (1ULL << 0U)
#define VMM_WRITE       (1ULL << 1U)
#define VMM_USER        (1ULL << 2U)
#define VMM_PWT         (1ULL << 3U)
#define VMM_PCD         (1ULL << 4U)
#define VMM_ACCESSED    (1ULL << 5U)
#define VMM_DIRTY       (1ULL << 6U)
#define VMM_HUGE        (1ULL << 7U)
#define VMM_GLOBAL      (1ULL << 8U)
#define VMM_NX          (1ULL << 63U)

#define PAGE_SIZE       0x1000
#define PAGE_SIZE_2M    0x200000
#define PAGE_SIZE_1G    0x40000000

extern void *kernel_start;
extern void *kernel_end;

void vmm_init();
void vmm_map_range(uint64_t *pml4, void *virt, size_t size, uint64_t flags);
void vmm_map_range_phys(uint64_t *pml4, void *virt, void *phys, size_t size, uint64_t flags);