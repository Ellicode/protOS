/**
 * @file        pmm.c
 * @brief       Physical Memory Manager source file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
*/

#include <stddef.h>
#include "debug/logger.h"
#include "mem/pmm.h"

pmm_t g_pmm = { .is_defined = 0 };

// Allocates a singular page
void *palloc() {
    if (g_pmm.is_defined == 0) {
        kwarn("PMM is not yet initialized");
        return NULL;
    }
    if (g_pmm.alloc == NULL) {
        kwarn("PMM does not yet support ALLOC");
        return NULL;
    }
    return g_pmm.alloc();
}

// Frees a page pointer
void pfree(void *ptr) {
    if (g_pmm.is_defined == 0) {
        kwarn("PMM is not yet initialized");
        return;
    }
    if (g_pmm.free == NULL) {
        kwarn("PMM does not yet support FREE");
        return;
    }
    g_pmm.free(ptr);
}

// Get the total memory size
size_t psize() {
    if (g_pmm.is_defined == 0) {
        kwarn("PMM is not yet initialized");
        return -1;
    }
    if (g_pmm.size == NULL) {
        kwarn("PMM does not yet support SIZE");
        return -1;
    }
    return g_pmm.size();
}

// Get the amount used memory
size_t pused() {
    if (g_pmm.is_defined == 0) {
        kwarn("PMM is not yet initialized");
        return -1;
    }
    if (g_pmm.used == NULL) {
        kwarn("PMM does not yet support USED");
        return -1;
    }
    return g_pmm.used();
}