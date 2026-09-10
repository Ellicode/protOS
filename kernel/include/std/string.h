/**
 * @file        string.c
 * @brief       Standard string operations header file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

#pragma once
#include <stdarg.h>
#include <stddef.h>

void *memmove(void *dest, const void *src, size_t len);
int memcmp(const void *str1, const void *str2, size_t count);
void *memset(void *dest, int val, size_t len);
void *memcpy(void *dest, const void *src, size_t len);
size_t strlen(const char* str);
char* strcat(char *dest, const char *src);
int strcmp(const char * s1, const char * s2);
char *strtok(char *src, const char *delim);
char* strcpy(char* dst, const char* src);
long strtol(const char *nptr, char **endptr, int base);
int strncmp(const char *s1, const char *s2, size_t n);
char *strncpy(char *dst, const char *src, size_t n);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
int atoi(const char *nptr);
int vsnprintf(char *str, size_t size, const char *format, va_list args);
int snprintf(char *str, size_t size, const char *format, ...);