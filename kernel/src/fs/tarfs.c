/**
 * @file        tarfs.c
 * @brief       Init TAR filesystem source file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

#include "debug/logger.h"
#include "std/string.h"
#include "mem/heap.h"
#include "fs/tarfs.h"
#include "boot.h"
#include "utils/align.h"

superblock_t *g_tarfs_sb;
tarfs_node_t *g_tarfs_root;

mode_t us_types[] = {
    [USTAR_NORMAL_ALT]      = S_IFREG,
    [USTAR_NORMAL]          = S_IFREG,
    [USTAR_HARD_LINK]       = S_IFLNK,
    [USTAR_SYMBOLIC_LINK]   = S_IFLNK,
    [USTAR_CHAR_DEV]        = S_IFCHR,
    [USTAR_BLOCK_DEV]       = S_IFBLK,
    [USTAR_DIRECTORY]       = S_IFDIR,
    [USTAR_PIPE]            = S_IFIFO,
};

static tarfs_node_t *tarfs_find_child(tarfs_node_t *parent, const char *name) {
    if (strcmp(name, "..") == 0) { return parent->parent; }

    for (tarfs_node_t *child = parent->child; child; child = child->next) {
        if (strcmp(child->name, name) == 0) { return child; }
    }

    return NULL;
}

static tarfs_node_t *tarfs_create_node(tarfs_node_t *parent, const char *name, ustar_node_t *us_node) {
    tarfs_node_t *existing = tarfs_find_child(parent, name);
    if (existing) { return existing; }

    char *size_endptr;
    off_t parsed_size = strtol(us_node->file_size, &size_endptr, 8);
    if (size_endptr == us_node->file_size) { return NULL; }

    tarfs_node_t *node = kalloc(sizeof(tarfs_node_t));
    if (node == NULL) { return NULL; }
    strcpy(node->name, name);
    node->parent            = parent;
    node->inode.type        = us_types[(uint32_t)us_node->type];
    node->inode.size        = parsed_size;
    node->inode.data        = node;
    node->inode.parent_sb   = g_tarfs_sb;
    node->inode.id          = ++g_curr_ino;

    node->next = parent->child;
    parent->child = node;
    return node;
}

static tarfs_node_t *tarfs_map_path(tarfs_node_t *root, ustar_node_t *header) {
    tarfs_node_t *current = root;

    char *saveptr = NULL;
    char *component = strtok_r(header->file_name, "/", &saveptr);

    while (component) {
        if (component[0] == '\0' || strcmp(component, ".") == 0) {
            component = strtok_r(NULL, "/", &saveptr);
            continue;
        }

        char *next = strtok_r(NULL, "/", &saveptr);
        tarfs_node_t *child = tarfs_find_child(current, component);

        if (!child) {
            ustar_node_t node = *header;

            if (next != NULL) {
                node.type = USTAR_DIRECTORY;
                strcpy(node.file_size, "00000000000");
            }

            if (!tarfs_create_node(current, component, &node)) { return NULL; }
        }

        current = child;
        component = next;
    }

    return current;
}

static int tarfs_lookup(inode_t *parent, const char *name, inode_t **out) {
    tarfs_node_t *node = parent->data;
    tarfs_node_t *out_node = tarfs_find_child(node, name);
    if (out_node == NULL) { return 2; } // ENOENT
    *out = &out_node->inode;
    return 0;
}

superblock_t *tarfs_init() {
    if (g_boot_info.modules_count == 0) { 
        kerror("No boot modules detected. Please check your %s bootloader configuration.", g_boot_info.protocol);
        return NULL;
    }

    g_tarfs_root = kalloc(sizeof(tarfs_node_t));
    g_tarfs_root->inode.type        = S_IFDIR;
    g_tarfs_root->inode.data        = g_tarfs_root;
    g_tarfs_root->inode.id          = ++g_curr_ino;

    g_tarfs_sb = kalloc(sizeof(superblock_t));
    g_tarfs_sb->fs_name             = "TARFS";
    g_tarfs_sb->ops.lookup          = tarfs_lookup;
    g_tarfs_sb->root                = &g_tarfs_root->inode;

    g_tarfs_root->inode.parent_sb   = g_tarfs_sb;

    boot_module_t initrd = g_boot_info.modules[0];

    ustar_node_t *header = (ustar_node_t *)initrd.address;
    off_t offset = 0;

    while (offset < initrd.size) {
        char *size_endptr;
        off_t parsed_size = strtol(header->file_size, &size_endptr, 8);
        if (size_endptr == header->file_size) { 
            offset += 512 + ROUND_UP(parsed_size, 512);
            header = (ustar_node_t *)((uintptr_t)initrd.address + offset);
            continue;
        }
        
        tarfs_map_path(g_tarfs_sb->root->data, header);

        offset += 512 + ROUND_UP(parsed_size, 512);
        header = (ustar_node_t *)((uintptr_t)initrd.address + offset);
    }   

    return g_tarfs_sb;
}