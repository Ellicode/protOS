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

#define S_IFMT      (0170000) 
#define S_IFSOCK    (0140000) // Socket
#define S_IFLNK     (0120000) // Symbolic link
#define S_IFREG     (0100000) // Regular file
#define S_IFBLK     (0060000) // Block device
#define S_IFDIR     (0040000) // Directory
#define S_IFCHR     (0020000) // Character device
#define S_IFIFO     (0010000) // FIFO

typedef struct inode_t {
    ino_t               id;

    off_t               size;
    mode_t              type;

    mode_t              mode;
    uid_t               uid;
    gid_t               gid;
    struct superblock_t *parent_sb;
    struct superblock_t *child_sb;

    void                *data;
} inode_t;

typedef struct file_descriptor_t {
    inode_t     *inode;
    off_t       off;
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
    char        *fs_name;
} superblock_t;

#define PATH_MAX        4096
#define PATH_DELIM      '/'

extern ino_t g_curr_ino;
extern superblock_t *g_rootfs;

void vfs_init();