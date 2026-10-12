/**
 * @file        vfs.c
 * @brief       Virtual filesystem source file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

#include "utils/ll.h"
#include "mem/heap.h"
#include "std/string.h"
#include "fs/vfs.h"
#include "fs/tarfs.h"
#include "debug/logger.h"

ino_t g_curr_ino = 0;
superblock_t *g_rootfs = NULL;

inode_t *vfs_lookup(inode_t *parent, const char *path) {
    if (!parent->parent_sb) {
        kwarn("inode %d does not have a parent superblock", parent->id);
        return NULL;
    }
    if (!parent->parent_sb->ops.lookup) {
        kwarn("filesystem \"%s\" does not support LOOKUP", parent->parent_sb->fs_name);
        return NULL;
    }

    inode_t *out;
    int res = parent->parent_sb->ops.lookup(parent, path, &out);

    if (res != 0) { return NULL; }

    return out;
}

inode_t *vfs_resolve_path(inode_t *parent, const char *path) {
    inode_t *current = parent;
    if (path[0] == PATH_DELIM) {
        if (strlen(path) == 1) { 
            return g_rootfs->root; 
        }
        current = g_rootfs->root;
    }

    char *saveptr = NULL;
    char *temp_path = strdup(path);
    char *component = strtok_r(temp_path, "/", &saveptr);

    while (component) {
        if (component[0] == '\0' || strcmp(component, ".") == 0) {
            component = strtok_r(NULL, "/", &saveptr);
            continue;
        }

        char *next = strtok_r(NULL, "/", &saveptr);
        current = vfs_lookup(current, component);
        if (current == NULL) { break; }

        component = next;
    }

    kfree(temp_path);
    return current;
}

void vfs_init() {
    g_rootfs = tarfs_init();
    ksuccess("Mounted \"%s\" filesystem at \"%s\".", g_rootfs->fs_name, "/");

    kassert("test path /system/bin/../bin/ls");
    inode_t *res = vfs_resolve_path(g_rootfs->root, "/system/bin/../bin/ls");
    if (res == NULL) { return; }
    kassert("inode { id: %d, size: %d bytes, name: \"%s\" }", res->id, res->size, ((tarfs_node_t *)res->data)->name);
}