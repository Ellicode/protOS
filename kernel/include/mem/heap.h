/**
 * @file        heap.h
 * @brief       Heap allocator header file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
*/

#pragma once
#include <stdint.h>
#include <stddef.h>

#define K_HEAP_VIRTUAL_START 0xFFFFA00000000000ULL

#define K_HEAP_MIN_SIZE      0x0000000000400000ULL // 4 MiB
#define K_HEAP_MAX_SIZE      0x0000100000000000ULL

#define HEAP_FREE (1U >> 0U)

typedef struct heap_node_t {
    size_t              size;
    uint8_t             flags;
    struct heap_node_t* prev;
    struct heap_node_t* next;
} __attribute__((packed)) heap_node_t;

void heap_init();
void* kalloc(size_t size);
void kfree(void *ptr);