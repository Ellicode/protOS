/**
 * @file        vfs.h
 * @brief       Virtual filesystem header file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

#pragma once
#include <stddef.h>
#include <stdint.h>
#include "types.h"

typedef enum {
    INODE_FILE,
    INODE_FOLDER,
    INODE_SYMLINK,
} inode_type_t;

typedef struct inode_t {
    uint64_t            id;

    inode_type_t        type;
    id_t                uid;
    id_t                gid;
    struct superblock_t *parent_sb;
    struct superblock_t *child_sb;

    void                *data;
} inode_t;

typedef struct file_descriptor_t {
    inode_t     *inode;
    uint64_t    off;
    uint16_t    flags;

    int         instances;
} file_descriptor_t;


typedef struct vfs_ops_t {
    int (*lookup) (inode_t *parent, const char *name, inode_t **out);

    int (*seek) (file_descriptor_t *fd, const void *off, size_t count, int whence);
    size_t (*read) (file_descriptor_t *fd, void *buffer, size_t count);
    size_t (*write) (file_descriptor_t *fd, const void *buffer, size_t count);
} vfs_ops_t;


typedef struct superblock_t {
    inode_t     *root;
    vfs_ops_t   ops;
} superblock_t;
