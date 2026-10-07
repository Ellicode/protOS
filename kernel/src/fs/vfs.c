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

inode_t *vfs_resolve_path(inode_t *parent, const char *path) {
    inode_t *cwd = parent;
    if (path[0] == PATH_DELIM) {
        if (strlen(path) == 1) { 
            return g_rootfs->root; 
        }
        cwd = g_rootfs->root;
    }

    uint8_t last = 0;
    char *tmp_path = strdup(path);
    if (tmp_path == NULL) { return NULL; }
    char *current_ptr = tmp_path;

    while (!last) {
        char *next = strchr(current_ptr, PATH_DELIM);
        if (next == NULL) { 
            last = 1;
            next = strchr(current_ptr, '\0');
        }
        
        size_t len = next - current_ptr;
        if (len == 0) { 
            current_ptr++;
            continue;
        }

        char seg[len + 1];
        strncpy(seg, current_ptr, len);
        seg[len] = '\0';

        current_ptr += last ? len : len + 1;

        kassert("%s, %d", seg, len);
    }

    kfree(tmp_path);
    return NULL;
}

int vfs_mount(superblock_t *child_sb, char *path) {

}

void vfs_init() {
    g_rootfs = tarfs_init();
    ksuccess("Mounted \"%s\" filesystem at \"%s\".", g_rootfs->fs_name, "/");
}