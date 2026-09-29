/**
 * @file        heap.c
 * @brief       Heap allocator source file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
*/

#include "mem/vmm.h"
#include "mem/heap.h"
#include "boot.h"
#include "utils/ticketlock.h"
#include "std/string.h"
#include "debug/logger.h"

heap_node_t *heap_root;
uint64_t heap_size = K_HEAP_MIN_SIZE;
ticketlock_t heap_lock = { 0 };

void* kalloc(size_t size) {
    heap_node_t *node = heap_root;
    int lock = ticketlock_lock(&heap_lock);
    while (node != NULL) {
        if (node->size >= size && node->flags & HEAP_FREE) {
            size_t remainder = node->size - size;
            if (remainder > sizeof(heap_node_t)) {
                heap_node_t *new = (void *)node + sizeof(heap_node_t) + size;
                new->size   = remainder - sizeof(heap_node_t);
                new->flags  = HEAP_FREE;
                new->prev   = node;
                new->next   = node->next;

                if (node->next != NULL) {
                    node->next->prev = new;
                }
                node->next = new;
            }
            node->size = size;
            node->flags &= ~HEAP_FREE;

            ticketlock_unlock(&heap_lock, lock);

            void *ptr = (void *)node + sizeof(heap_node_t);
            memset(ptr, 0, size);
            return ptr;
        }
        node = node->next;
    }

    if (heap_size < K_HEAP_MAX_SIZE) {
        heap_node_t *last = heap_root;
        while (last->next != NULL) {
            last = last->next;
        }

        vmm_map_range(g_kern_pml4, (void *)K_HEAP_VIRTUAL_START + heap_size, size + sizeof(heap_node_t), VMM_WRITE);

        heap_node_t *new = (heap_node_t*)(K_HEAP_VIRTUAL_START + heap_size);
        new->size   = size;
        new->flags  = HEAP_FREE;
        new->prev   = last;
        new->next   = NULL;
        last->next  = new;

        heap_size += size + sizeof(heap_node_t);
    }

    ticketlock_unlock(&heap_lock, lock);
    return NULL;
}

void kfree(void *ptr) {
    if (ptr == NULL) { return; }
    int lock = ticketlock_lock(&heap_lock);

    heap_node_t *block = (heap_node_t*)(ptr - sizeof(heap_node_t));

    if (block->flags & HEAP_FREE) {
        ticketlock_unlock(&heap_lock, lock);
        return;
    }

    block->flags |= HEAP_FREE;

    if (block->next != NULL && (block->next->flags & HEAP_FREE)) {
        block->size += sizeof(heap_node_t) + block->next->size;
        block->next  = block->next->next;

        if (block->next != NULL) {
            block->next->prev = block;
        }
    }

    if (block->prev != NULL && (block->prev->flags & HEAP_FREE)) {
        block->prev->size += sizeof(heap_node_t) + block->size;
        block->prev->next  = block->next;

        if (block->next != NULL) {
            block->next->prev = block->prev;
        }
    }

    ticketlock_unlock(&heap_lock, lock);
}

void heap_init() {
    vmm_map_range(g_kern_pml4, (void *)K_HEAP_VIRTUAL_START, K_HEAP_MIN_SIZE, VMM_WRITE);
    heap_root = (heap_node_t *)K_HEAP_VIRTUAL_START;
    heap_root->flags = HEAP_FREE;
    heap_root->prev = NULL;
    heap_root->next = NULL;
    heap_root->size = K_HEAP_MIN_SIZE;
}