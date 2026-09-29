/**
 * @file        vmm.c
 * @brief       Virtual Memory Manager source file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
*/

#include "debug/logger.h"
#include "mem/vmm.h"
#include "mem/pmm.h"
#include "utils/align.h"
#include "utils/ticketlock.h"
#include "utils/msr.h"
#include "boot.h"
#include "_deps/limine.h"

uint64_t *g_kern_pml4 __attribute__((aligned(PAGE_SIZE)));
ticketlock_t vmm_lock = { 0 };

static inline void load_cr3(void* cr3) {
    __asm__ volatile("mov %0, %%cr3" :: "r"((uint64_t)cr3) : "memory");
}

static inline void invlpg(void *virt) {
    __asm__ volatile("invlpg (%0)" :: "r"(virt) : "memory");
}

uint64_t* vmm_get_or_create_table(uint64_t *entry, uint64_t flags) {
    if (*entry & VMM_PRESENT) {
        *entry |= VMM_WRITE | flags;
        uint64_t phys = ((*entry >> 12U) & 0x1FFFFFFFFULL) << 12U;
        return (uint64_t*)(phys + g_boot_info.hhdm);
    }

    void *table_phys = palloc(); // Allocate new page table
    if (table_phys == NULL) { return NULL; }

    uint64_t *table_virt = (uint64_t *)(table_phys + g_boot_info.hhdm);

    *entry = ((uint64_t)table_phys & 0x000FFFFFFFFFF000ULL) | VMM_PRESENT | VMM_WRITE | flags;

    return table_virt;
}

void vmm_map_page(uint64_t *pml4, void *virt, void *phys, uint64_t flags) {
    if (pml4 == NULL) { kerror("Tried to map before paging was initialized"); return; }

    uint16_t pml4_off   = ((uint64_t)virt >> 39U) & 0x1FFU;
    uint16_t pdpt_off   = ((uint64_t)virt >> 30U) & 0x1FFU;
    uint16_t pd_off     = ((uint64_t)virt >> 21U) & 0x1FFU;
    uint16_t pt_off     = ((uint64_t)virt >> 12U) & 0x1FFU;

    uint64_t* pdpt = vmm_get_or_create_table(&pml4[pml4_off], flags);
    if (pdpt == NULL) { kerror("Could not alloc PDPT table"); return; }

    uint64_t* pd   = vmm_get_or_create_table(&pdpt[pdpt_off], flags);
    if (pd == NULL) { kerror("Could not alloc PD table"); return; }

    uint64_t* pt   = vmm_get_or_create_table(&pd[pd_off], flags);
    if (pt == NULL) { kerror("Could not alloc PT table"); return; }

    uint64_t* entry = &pt[pt_off];
    *entry = ((uint64_t)phys & 0x000FFFFFFFFFF000ULL) | VMM_PRESENT | flags;

    invlpg(virt);
}

void vmm_map_page_1g(uint64_t *pml4, void *virt, void *phys, uint64_t flags) {
    if (pml4 == NULL) { kerror("ried to map before paging was initialized"); return; }

    uint16_t pml4_off   = ((uint64_t)virt >> 39U) & 0x1FFU;
    uint16_t pdpt_off   = ((uint64_t)virt >> 30U) & 0x1FFU;

    uint64_t* pdpt = vmm_get_or_create_table(&pml4[pml4_off], flags);
    if (pdpt == NULL) { kerror("Could not alloc PDPT table"); return; }

    uint64_t* entry = &pdpt[pdpt_off];
    *entry = ((uint64_t)phys & 0x000FFFFF40000000ULL) | VMM_PRESENT | VMM_HUGE | flags;

    invlpg(virt);
}

void vmm_map_page_2m(uint64_t *pml4, void *virt, void *phys, uint64_t flags) {
    if (pml4 == NULL) { kerror("Tried to map before paging was initialized"); return; }

    uint16_t pml4_off   = ((uint64_t)virt >> 39U) & 0x1FFU;
    uint16_t pdpt_off   = ((uint64_t)virt >> 30U) & 0x1FFU;
    uint16_t pd_off     = ((uint64_t)virt >> 21U) & 0x1FFU;

    uint64_t* pdpt = vmm_get_or_create_table(&pml4[pml4_off], flags);
    if (pdpt == NULL) { kerror("Could not alloc PDPT table"); return; }

    uint64_t* pd = vmm_get_or_create_table(&pdpt[pdpt_off], flags);
    if (pd == NULL) { kerror("Could not alloc PD table"); return; }

    uint64_t* entry = &pd[pd_off];
    *entry = ((uint64_t)phys & 0x000FFFFFFFE00000ULL) | VMM_PRESENT | VMM_HUGE | flags;

    invlpg(virt);
}

void vmm_map_range(uint64_t *pml4, void *virt, size_t size, uint64_t flags) {
    virt = (void *)PAGE_ALIGN(virt);
    size_t rounded_size = PAGE_ROUND(size);

    int lock = ticketlock_lock(&vmm_lock);

    for (size_t i = 0; i < rounded_size; i += PAGE_SIZE) {
        void *page = palloc();
        if (page == NULL) { ticketlock_unlock(&vmm_lock, lock); return; };
        
        vmm_map_page(pml4, virt + i, page, flags);
    }

    ticketlock_unlock(&vmm_lock, lock);
}

void vmm_map_range_phys(uint64_t *pml4, void *virt, void *phys, size_t size, uint64_t flags) {
    uint64_t v = (uint64_t)virt;
    uint64_t p = (uint64_t)phys;
    size_t remaining = PAGE_ROUND(size);

    int lock = ticketlock_lock(&vmm_lock);

    while (remaining > 0) {
        if (remaining >= PAGE_SIZE_1G && (v % PAGE_SIZE_1G == 0) && (p % PAGE_SIZE_1G == 0)) {
            vmm_map_page_1g(pml4, (void*)v, (void*)p, flags);
            v += PAGE_SIZE_1G;
            p += PAGE_SIZE_1G;
            remaining -= PAGE_SIZE_1G;
        } else if (remaining >= PAGE_SIZE_2M && (v % PAGE_SIZE_2M == 0) && (p % PAGE_SIZE_2M == 0)) {
            vmm_map_page_2m(pml4, (void*)v, (void*)p, flags);
            v += PAGE_SIZE_2M;
            p += PAGE_SIZE_2M;
            remaining -= PAGE_SIZE_2M;
        } else {
            vmm_map_page(pml4, (void*)v, (void*)p, flags);
            v += PAGE_SIZE;
            p += PAGE_SIZE;
            remaining -= PAGE_SIZE;
        }
    }

    ticketlock_unlock(&vmm_lock, lock);
}

void pat_set_entry(uint64_t *pat, int idx, int pat_num) {
    *pat &= ~(0xFFULL << (idx * 8U));
    *pat |=  ((uint64_t)pat_num << (idx * 8U));
}

void vmm_init() {
    uint64_t pat = rdmsr(0x277);
    pat_set_entry(&pat, 0, 0); // UC
    pat_set_entry(&pat, 1, 1); // WC
    wrmsr(0x277, pat);

    void *pml4_phys = palloc(); // Allocate pml4
    if (pml4_phys == NULL) { kerror("Could not allocate kernel PML4 table"); return; }

    g_kern_pml4 = pml4_phys + g_boot_info.hhdm;
    g_kern_pml4[510] = ((uint64_t)pml4_phys & 0x000FFFFFFFFFF000ULL) | VMM_PRESENT | VMM_WRITE;

    // Map kernel
    size_t ksize = (uint64_t)&kernel_end - (uint64_t)&kernel_start;
    vmm_map_range_phys(g_kern_pml4, g_boot_info.kernel_virt, g_boot_info.kernel_phys, ksize, VMM_WRITE);

    // Map hhdm
    for (size_t i = 0; i < g_boot_info.memmap_entry_count; i++) {
        struct memmap_entry_t *entry = g_boot_info.memmap[i];
        vmm_map_range_phys(g_kern_pml4, (void *)entry->base + g_boot_info.hhdm, (void *)entry->base, entry->length, VMM_WRITE);
    }

    // Map framebuffer
    vmm_map_range_phys(
        g_kern_pml4, 
        g_boot_info.framebuffer.address, 
        g_boot_info.framebuffer.address - g_boot_info.hhdm, 
        g_boot_info.framebuffer.pitch * g_boot_info.framebuffer.height, 
        VMM_WRITE | VMM_WC
    );
    
    load_cr3(pml4_phys);
}