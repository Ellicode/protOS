/**
 * @file        pit.h
 * @brief       Programmable interval timer interface
 *
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
*/

#pragma once
#include <stdint.h>

extern uint32_t g_pit_ticks;

uint32_t pit_calibrate();