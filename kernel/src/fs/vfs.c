/**
 * @file        vfs.c
 * @brief       Virtual filesystem source file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

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

int kopen(inode_t *parent, const char *path, mode_t mode, file_descriptor_t **fd) {
    *fd = kalloc(sizeof(file_descriptor_t));
    if (*fd == NULL) { return 12; } // ENOMEM

    inode_t *inode = vfs_resolve_path(parent, path);
    if (inode == NULL) { return 2; } // ENOENT

    (*fd)->inode   = inode;
    (*fd)->off     = 0;

    return 0;
}

size_t kread(file_descriptor_t *fd, void *buffer, size_t count) {
    if (!fd->inode->parent_sb) {
        kwarn("inode %d does not have a parent superblock", fd->inode->id);
        return -1;
    }
    if (!fd->inode->parent_sb->ops.read) {
        kwarn("filesystem \"%s\" does not support READ", fd->inode->parent_sb->fs_name);
        return -1; // EPERM
    }

    size_t ret = fd->inode->parent_sb->ops.read(fd, buffer, count);
    fd->off += (off_t)ret;

    return ret;
}


void vfs_init() {
    g_rootfs = tarfs_init();
    ksuccess("Mounted \"%s\" filesystem at \"%s\".", g_rootfs->fs_name, "/");

    file_descriptor_t *fd;
    kopen(g_rootfs->root, "hello.txt", 0, &fd);
    kassert("name: \"%s\", size: %d, addr: %x", ((tarfs_node_t *)fd->inode->data)->name, fd->inode->size, ((tarfs_node_t *)fd->inode->data)->addr);

    char *buf = kalloc(fd->inode->size);
    int ret = kread(fd, buf, fd->inode->size);
    kassert("ret: %d, contents: %s", ret, buf);
}