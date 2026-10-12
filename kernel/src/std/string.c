/**
 * @file        string.c
 * @brief       Standard string operations source file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
 */

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "mem/heap.h"

void *memmove(void *dest, const void *src, size_t len) {
    char *d = dest;
    const char *s = src;
    if (d < s) {
        while (len--) {
            *d++ = *s++;
        }
    } else {
        const char *lasts = s + (len-1);
        char *lastd = d + (len-1);
        while (len--) {
            *lastd-- = *lasts--;
        }
    }
    return dest;
}

int memcmp(const void *str1, const void *str2, size_t count) {
    const unsigned char *s1 = str1;
    const unsigned char *s2 = str2;

    while (count-- > 0) {
        if (*s1++ != *s2++) {
            return s1[-1] < s2[-1] ? -1 : 1;
        }
    }
    return 0;
}

void *memset(void *dest, int val, size_t len) {
    unsigned char *ptr = dest;
    while (len-- > 0) {
        *ptr++ = val;
    }
    return dest;
}

void *memcpy(void *restrict dest, const void *restrict src, size_t len) {
    char *d = dest;
    const char *s = src;
    while (len--) {
        *d++ = *s++;
    }
    return dest;
}

size_t strlen(const char* str) {
	size_t len = 0;
	while (str[len]) {
		len++;
    }
	return len;
}

char* strcat(char *restrict dest, const char *restrict src) {
    size_t dest_len = strlen(dest);
    int i = 0;
    while (src[i] != '\0')
    {
        dest[dest_len + i] = src[i];
        i++;
    }
    dest[dest_len + i] = '\0';
    return dest;
}

int strcmp(const char * s1, const char * s2) {
    for (size_t i = 0; s1[i] != '\0' || s2[i] != '\0'; i++) {
        if (((char*)s1)[i] != ((char*)s2)[i]) {
            if (((char*)s1)[i] < ((char*)s2)[i]) {
                return -1;
            }
            return 1;
        }
    }
    return 0;
}

char *strtok_r(char *src, const char *delim, char **saveptr) {
    char *str;

    if (src != NULL) {
        str = src;
    } else {
        str = *saveptr;
    }

    if (str == NULL) {
        return NULL;
    }

    while (*str != '\0') {
        bool is_delim = false;

        for (size_t i = 0; delim[i] != '\0'; i++) {
            if (*str == delim[i]) {
                is_delim = true;
                break;
            }
        }

        if (!is_delim) {
            break;
        }

        str++;
    }

    if (*str == '\0') {
        *saveptr = str;
        return NULL;
    }

    char *token = str;

    while (*str != '\0') {
        for (size_t i = 0; delim[i] != '\0'; i++) {
            if (*str == delim[i]) {
                *str = '\0';
                *saveptr = str + 1;
                return token;
            }
        }

        str++;
    }

    *saveptr = str;
    return token;
}

char *strtok(char *src, const char *delim) {
    static char * last_tok = NULL;
    if (src != NULL) {
        last_tok = src;
    }
    if (last_tok == NULL) { 
        return NULL;
    }

    if (*last_tok == '\0') {
        return NULL;
    }

    for (size_t i = 0; last_tok[i] != '\0'; i++) {
        for (int j = 0; delim[j] != '\0'; j++) {
            if (last_tok[i] == delim[j]) {
                goto fail;
            }
        }
        last_tok += i;
        break;
        fail:
        continue;
    }

    size_t i = 0;
    for (i = 0; last_tok[i] != '\0'; i++) {
        for (int j = 0; delim[j] != '\0'; j++) {
            if (last_tok[i] == delim[j]) {
                goto found;
            }
        }
    }
    found:
    last_tok[i] = '\0';

    char * ret = last_tok;
    last_tok += i + 1;
    return ret;
}

char* strcpy(char *restrict dst, const char *restrict src) {
    const size_t length = strlen(src);
    memcpy(dst, src, length + 1);
    return dst;
}

long strtol(const char *nptr, char **endptr, int base) {
    const char *s = nptr;
    unsigned long acc = 0;
    int c;
    uint8_t neg = 0;
    uint8_t any = 0;

    while ((c = *s) == ' ' || (c == '\t') || (c == '\n') || 
           (c == '\v') || (c == '\f') || (c == '\r')) {
        s++;
    }

    if (*s == '-') {
        neg = 1;
        s++;
    } else if (*s == '+') {
        s++;
    }

    if (base == 0) {
        if (*s == '0' && (s[1] == 'x' || s[1] == 'X')) {
            s += 2;
            base = 16;
        } else if (*s == '0') {
            s++;
            base = 8;
        } else {
            base = 10;
        }
    } else if (base == 16 && *s == '0' && (s[1] == 'x' || s[1] == 'X')) {
        s += 2;
    }

    for (c = *s; ; c = *++s) {
        int val = -1;
        if (c >= '0' && c <= '9') {
            val = c - '0';
        } else if (c >= 'a' && c <= 'z') {
            val = c - 'a' + 10;
        } else if (c >= 'A' && c <= 'Z') {
            val = c - 'A' + 10;
        }

        if (val < 0 || val >= base) {
            break;
        }

        any = 1;
        acc = (acc * base) + val;
    }

    long res = (long)acc;
    if (neg) {
        res = -res;
    }

    if (endptr != NULL) {
        *endptr = (char *)(any ? s : nptr);
    }

    return res;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    while (n > 0) {
        if (*s1 != *s2) {
            return (unsigned char)*s1 - (unsigned char)*s2;
        }
        if (*s1 == '\0') {
            return 0;
        }
        s1++;
        s2++;
        n--;
    }
    return 0;
}

char *strncpy(char *restrict dst, const char *restrict src, size_t n) {
    if (n != 0) {
        char *d = dst;
        const char *s = src;

        do {
            char copied = *s++;
            *d++ = copied;
            if (copied == '\0') {
                while (--n != 0) {
                    *d++ = '\0';
                }
                break;
            }
        } while (--n != 0);
    }
    return dst;
}

char *strchr(const char *s, int c) {
    while (*s != '\0') {
        if (*s == (char)c) {
            return (char *)s;
        }
        s++;
    }

    if (c == '\0') {
        return (char *)s;
    }

    return NULL;
}

char *strrchr(const char *s, int c) {
    char *last_match = NULL;
    while (*s != '\0') {
        if (*s == (char)c) {
            last_match = (char *)s;
        }
        s++;
    }
    if (c == '\0') {
        return (char *)s;
    }

    return last_match;
}

char *strndup(const char *s, size_t n) {
    char *new = kalloc(n + 1);
    if (new == NULL) { return NULL; }
    strncpy(new, s, n);
    new[strlen(s)] = '\0';
    return new;
}

char *strdup(const char *s) {
    return strndup(s, strlen(s));
}

int atoi(const char *nptr) {
    return (int)strtol(nptr, (char **)NULL, 10);
}

static char *itos(int64_t num) {
    static char str[21];
    int i = 0;
    int isNegative = 0;
    if (num < 0) {
        isNegative = 1;
        num = -num;
    }

    do {
        str[i++] = (num % 10) + '0';
    } while ((num /= 10) > 0);

    if (isNegative) {
        str[i++] = '-';
    }
    str[i] = '\0';
    for (int j = 0; j < i / 2; j++){
        char temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }
    return str;
}

char *htos(uint64_t value) {
    static char str[17]; 
    const char *hex_digits = "0123456789ABCDEF";

    if (value == 0) {
        str[0] = '0';
        str[1] = '\0';
        return str;
    }

    str[16] = '\0';
    
    int i = 15; 
    
    while (value != 0) {
        str[i--] = hex_digits[value & 0xFU];
        value >>= 4U;
    }

    return &str[i + 1];
};

static char *dtos(double num, int precision, char *buffer) {
    int pos = 0;

    if (num < 0.0) {
        buffer[pos++] = '-';
        num = -num;
    }

    double rounding = 0.5;
    for (int i = 0; i < precision; ++i) {
        rounding /= 10.0;
    }
    num += rounding;

    long long int_part = (long long)num;
    double frac_part = num - (double)int_part;

    int int_start = pos;
    if (int_part == 0) {
        buffer[pos++] = '0';
    } else {
        long long temp = int_part;
        while (temp > 0) {
            buffer[pos++] = (char)((temp % 10) + '0');
            temp /= 10;
        }
        for (int i = int_start, j = pos - 1; i < j; i++, j--) {
            char temp_char = buffer[i];
            buffer[i] = buffer[j];
            buffer[j] = temp_char;
        }
    }

    if (precision > 0) {
        buffer[pos++] = '.';
        for (int i = 0; i < precision; i++) {
            frac_part *= 10.0;
            int digit = (int)frac_part;
            buffer[pos++] = (char)(digit + '0');
            frac_part -= (double)digit;
        }
    }

    buffer[pos] = '\0';
    return buffer;
}

int vsnprintf(char *str, size_t size, const char *format, va_list args) {
    size_t written = 0;
    char float_buf[64];

    // Helper macro to centralize buffer safety checks and character counting
    #define EMIT(c) do { \
        if (size > 0 && written < size - 1) { \
            str[written] = (c); \
        } \
        written++; \
    } while(0)

    for (size_t i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%') {
            i++;
            if (format[i] == '\0') { break; }

            bool left_align = false;
            char pad_char = ' ';
            size_t width = 0;
            int precision = -1; 

            while (format[i] == '-' || format[i] == '0') {
                if (format[i] == '-') { left_align = true; }
                if (format[i] == '0') { pad_char = '0'; }
                i++;
            }
            if (left_align) { pad_char = ' '; }

            while (format[i] >= '0' && format[i] <= '9') {
                width = (width * 10) + (format[i] - '0');
                i++;
            }

            if (format[i] == '.') {
                i++;
                precision = 0;
                while (format[i] >= '0' && format[i] <= '9') {
                    precision = (precision * 10) + (format[i] - '0');
                    i++;
                }
            }

            // 4. Evaluate specifier
            const char *arg_str = NULL;
            size_t len = 0;

            if (format[i] == 'd') {
                int value = va_arg(args, int);
                arg_str = itos(value);
                len = strlen(arg_str);
            } else if (format[i] == 'x') {
                uint64_t value = va_arg(args, uint64_t);
                arg_str = htos(value);
                len = strlen(arg_str);
            } else if (format[i] == 's') {
                arg_str = va_arg(args, char *);
                if (arg_str == NULL) {
                    arg_str = "(null)";
                }
                len = strlen(arg_str);
                // Apply string precision (max characters to print)
                if (precision >= 0 && (size_t)precision < len) {
                    len = precision;
                }
            } else if (format[i] == 'f') {
                double value = va_arg(args, double);
                // Apply float precision (defaults to 6)
                int p = (precision >= 0) ? precision : 6; 
                arg_str = dtos(value, p, float_buf);
                len = strlen(arg_str);
            } else if (format[i] == '%') {
                EMIT('%');
                continue;
            } else {
                // Unknown specifier: print literally
                EMIT('%');
                EMIT(format[i]);
                continue;
            }

            // 5. Output with alignment and padding
            size_t pad_len = (width > len) ? (width - len) : 0;

            if (!left_align) {
                for (size_t p = 0; p < pad_len; p++) { EMIT(pad_char); }
            }

            for (size_t c = 0; c < len; c++) {
                EMIT(arg_str[c]);
            }

            if (left_align) {
                for (size_t p = 0; p < pad_len; p++) { EMIT(' '); }
            }
        } else {
            EMIT(format[i]);
        }
    }

    #undef EMIT

    // Safely null-terminate if the buffer is provided
    if (size > 0) {
        if (written < size) {
            str[written] = '\0';
        } else {
            str[size - 1] = '\0';
        }
    }

    return (int)written;
}

int snprintf(char *str, size_t size, const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    int result = vsnprintf(str, size, format, args);
    
    va_end(args);
    return result;
}