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

#define klog(format, color, name, ...) kassert("\e[%dm%4s\e[37m::%-16s " format, color, name, __func__, ##__VA_ARGS__)

#define kerror(format, ...)     klog(format, 31, "err", ##__VA_ARGS__)
#define kwarn(format, ...)      klog(format, 33, "warn", ##__VA_ARGS__)
#define kinfo(format, ...)      klog(format, 34, "info", ##__VA_ARGS__)
#define ksuccess(format, ...)   klog(format, 32, "ok", ##__VA_ARGS__)

#ifdef PROTO_DEBUG
#define kdebug(format, ...)     klog(format, 35, "dbug", ##__VA_ARGS__)
#else
#define kdebug(format, ...)
#endif