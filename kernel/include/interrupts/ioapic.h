/**
 * @file        ioapic.h
 * @brief       IOAPIC header file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
*/

#pragma once
#include "interrupts/apic.h"

extern void *g_ioapic_base;

void ioapic_init(madt_ioapic_entry *entry);