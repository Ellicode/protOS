/**
 * @file        logger.h
 * @brief       Logging debug utilites header file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

#pragma once
#include "ui/console.h"
#include "cpu/sse.h"

#define kassert(format, ...) do { \
    kprintf("[%11d] " format "\n", 0, ##__VA_ARGS__); \
} while (0)

#define kerror(format, ...)     kassert("\e[31m err\e[0m: <%s:%d> " format, __FILE_NAME__, __LINE__, ##__VA_ARGS__)
#define kwarn(format, ...)      kassert("\e[33mwarn\e[0m: <%s:%d> " format, __FILE_NAME__, __LINE__, ##__VA_ARGS__)
#define kinfo(format, ...)      kassert("\e[34minfo\e[0m: <%s:%d> " format, __FILE_NAME__, __LINE__, ##__VA_ARGS__)
#define ksuccess(format, ...)   kassert("\e[32m  ok\e[0m: <%s:%d> " format, __FILE_NAME__, __LINE__, ##__VA_ARGS__)

#ifdef PROTO_DEBUG
#define kdebug(format, ...)     kassert("\e[35mdbug\e[0m: <%s:%d> " format, __FILE_NAME__, __LINE__, ##__VA_ARGS__)
#else
#define kdebug(format, ...)
#endif