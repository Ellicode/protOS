/**
 * @file        tarfs.h
 * @brief       Init TAR filesystem header file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

#pragma once
#include "fs/vfs.h"
#include "types.h"

enum {
    USTAR_NORMAL_ALT    = 0,
    USTAR_NORMAL        = '0',
    USTAR_HARD_LINK     = '1',
    USTAR_SYMBOLIC_LINK = '2',
    USTAR_CHAR_DEV      = '3',
    USTAR_BLOCK_DEV     = '4',
    USTAR_DIRECTORY     = '5',
    USTAR_PIPE          = '6',
};

typedef struct __attribute__((packed)) {
    char    file_name[100];
    char    mode[8];
    char    owner_id[8];
    char    group_id[8];
    char    file_size[12];
    char    last_modified_time[12];
    char    checksum[8];
    char    type;
    char    linked_file_name[100];
    char    ustar_magic[6];
    char    ustar_version[2];
    char    owner_name[32];
    char    group_name[32];
    char    dev_major[8];
    char    dev_minor[8];
    char    filename_prefix[155];
    char    reserved[12];
} ustar_node_t;

typedef struct tarfs_node_t {
    char                    name[100];
    off_t                   offset;
    inode_t                 inode;
    struct tarfs_node_t*    parent;
    struct tarfs_node_t*    child;
    struct tarfs_node_t*    next;
} tarfs_node_t;

superblock_t *tarfs_init();