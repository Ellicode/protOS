/**
 * @file        pmm.h
 * @brief       Physical Memory Manager header file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
*/

#pragma once
#include <stddef.h>

typedef struct pmm_t {
    int     is_defined;
    char    type[16];

    void    *(*alloc)();
    void    (*free)(void *);

    size_t  (*size)();
    size_t  (*used)();
} pmm_t;

extern pmm_t g_pmm;

void *palloc();
void pfree(void *ptr);

size_t psize();
size_t pused();