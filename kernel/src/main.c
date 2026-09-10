/**
 * @file        main.c
 * @brief       Main entry point for the protOS kernel
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

#include "boot.h"

// Main entry point of the protOS kernel
void kmain() {
    kboot();
}