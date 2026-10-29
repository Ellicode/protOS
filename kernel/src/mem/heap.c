
/**
 * @file       heap.c
 * @brief      Heap allocator source file
 *
 * @author     Elliot Laborieux
 * @copyright  Copyright (c) 2026 Ellicode
 */

#include <stddef.h>
#include <stdint.h>
#include <limits.h>

#include "mem/vmm.h"
#include "mem/heap.h"
#include "boot.h"
#include "utils/ticketlock.h"
#include "std/string.h"
#include "debug/logger.h"

#define HEAP_ALIGNMENT 16
#define HEAP_PAGE_SIZE 4096

heap_node_t *heap_root = NULL;
uint64_t heap_size = K_HEAP_MIN_SIZE;
ticketlock_t heap_lock = { 0 };
char is_heap_init = 0;

static size_t heap_align_up(size_t size, size_t alignment) {
    if (size > SIZE_MAX - (alignment - 1)) {
        return 0;
    }

    return (size + alignment - 1) & ~(alignment - 1);
}

static void *heap_alloc_node(heap_node_t *node, size_t size) {
    size_t remainder = node->size - size;

    if (remainder >= sizeof(heap_node_t) + HEAP_ALIGNMENT) {
        heap_node_t *next = (heap_node_t *)(
            (uintptr_t)node + sizeof(heap_node_t) + size
        );

        next->size = remainder - sizeof(heap_node_t);
        next->flags = HEAP_FREE;
        next->prev = node;
        next->next = node->next;

        if (next->next != NULL) {
            next->next->prev = next;
        }

        node->next = next;
        node->size = size;
    }

    node->flags &= ~HEAP_FREE;

    return (void *)((uintptr_t)node + sizeof(heap_node_t));
}

static void *heap_expand(size_t size) {
    if (heap_size >= K_HEAP_MAX_SIZE) {
        return NULL;
    }

    if (size > SIZE_MAX - sizeof(heap_node_t)) {
        return NULL;
    }

    size_t required = size + sizeof(heap_node_t);
    size_t growth = heap_align_up(required, HEAP_PAGE_SIZE);

    if (growth == 0) {
        return NULL;
    }

    uint64_t remaining = K_HEAP_MAX_SIZE - heap_size;

    if (growth > remaining) {
        growth = (size_t)(remaining & ~(uint64_t)(HEAP_PAGE_SIZE - 1));

        if (growth < required) {
            return NULL;
        }
    }

    uintptr_t base = (uintptr_t)K_HEAP_VIRTUAL_START;
    uintptr_t address = base + heap_size;

    vmm_map_range(
        g_kern_pml4,
        (void *)address,
        growth,
        VMM_WRITE
    );

    heap_node_t *node = (heap_node_t *)address;

    node->size = growth - sizeof(heap_node_t);
    node->flags = HEAP_FREE;
    node->prev = NULL;
    node->next = NULL;

    heap_node_t *last = heap_root;

    if (last == NULL) {
        heap_root = node;
    } else {
        while (last->next != NULL) {
            last = last->next;
        }

        last->next = node;
        node->prev = last;
    }

    heap_size += growth;

    return heap_alloc_node(node, size);
}

void *kalloc(size_t size) {
    if (!is_heap_init) {
        kwarn("Heap is not ready for alloc!");
        return NULL;
    }

    if (size == 0) {
        return NULL;
    }

    size_t aligned_size = heap_align_up(size, HEAP_ALIGNMENT);

    if (aligned_size == 0) {
        return NULL;
    }

    int lock = ticketlock_lock(&heap_lock);

    heap_node_t *node = heap_root;

    while (node != NULL) {
        if ((node->flags & HEAP_FREE) &&
            node->size >= aligned_size) {

            void *ptr = heap_alloc_node(node, aligned_size);

            ticketlock_unlock(&heap_lock, lock);

            memset(ptr, 0, size);
            return ptr;
        }

        node = node->next;
    }

    void *ptr = heap_expand(aligned_size);

    ticketlock_unlock(&heap_lock, lock);

    if (ptr != NULL) {
        memset(ptr, 0, size);
    }

    return ptr;
}

void kfree(void *ptr) {
    if (!is_heap_init) {
        kwarn("Heap is not ready for free!");
        return;
    }

    if (ptr == NULL) {
        return;
    }

    uintptr_t address = (uintptr_t)ptr;
    uintptr_t heap_start = (uintptr_t)K_HEAP_VIRTUAL_START;
    uintptr_t heap_end = heap_start + heap_size;

    /*
     * Reject pointers outside the heap's mapped region.
     */
    if (address < heap_start + sizeof(heap_node_t) ||
        address >= heap_end) {
        kwarn("kfree: pointer outside heap");
        return;
    }

    int lock = ticketlock_lock(&heap_lock);

    heap_node_t *block = (heap_node_t *)(
        address - sizeof(heap_node_t)
    );

    heap_node_t *node = heap_root;

    while (node != NULL && node != block) {
        node = node->next;
    }

    if (node == NULL) {
        ticketlock_unlock(&heap_lock, lock);
        kwarn("kfree: invalid allocation pointer");
        return;
    }

    if (block->flags & HEAP_FREE) {
        ticketlock_unlock(&heap_lock, lock);
        kwarn("kfree: double free");
        return;
    }

    block->flags |= HEAP_FREE;

    if (block->next != NULL &&
        (block->next->flags & HEAP_FREE)) {

        heap_node_t *next = block->next;

        block->size += sizeof(heap_node_t) + next->size;
        block->next = next->next;

        if (block->next != NULL) {
            block->next->prev = block;
        }
    }

    if (block->prev != NULL &&
        (block->prev->flags & HEAP_FREE)) {

        heap_node_t *prev = block->prev;

        prev->size += sizeof(heap_node_t) + block->size;
        prev->next = block->next;

        if (block->next != NULL) {
            block->next->prev = prev;
        }
    }

    ticketlock_unlock(&heap_lock, lock);
}

void heap_init(void) {
    if (is_heap_init) {
        return;
    }

    if (K_HEAP_MIN_SIZE < sizeof(heap_node_t) ||
        K_HEAP_MIN_SIZE > K_HEAP_MAX_SIZE) {
        kwarn("Invalid heap size configuration");
        return;
    }

    vmm_map_range(
        g_kern_pml4,
        (void *)(uintptr_t)K_HEAP_VIRTUAL_START,
        K_HEAP_MIN_SIZE,
        VMM_WRITE
    );

    heap_root = (heap_node_t *)(uintptr_t)K_HEAP_VIRTUAL_START;

    heap_root->size = K_HEAP_MIN_SIZE - sizeof(heap_node_t);
    heap_root->flags = HEAP_FREE;
    heap_root->prev = NULL;
    heap_root->next = NULL;

    heap_size = K_HEAP_MIN_SIZE;
    is_heap_init = 1;
}
