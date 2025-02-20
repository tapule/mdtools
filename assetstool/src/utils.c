/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of mdtools (MegaDrive development tools).
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2025
 * Github: https://github.com/tapule/mdtools
 */

/**
 * \file            utils.c
 * \brief           General utility functions
 */

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>

inline void
fprintf_color_set(FILE *const restrict stream, const char *const restrict color) {
    fprintf(stream, color);
}

inline void
fprintf_color_reset(FILE *const restrict stream) {
    fprintf(stream, PRINT_COLOR_RESET);
}

void
utils_error(const char *const restrict msg, ...) {
    if (msg != nullptr) {
        va_list args;
        va_start(args);
        vfprintf(stderr, msg, args);
        va_end(args);
        fprintf(stderr, "\n");
    }
    exit(EXIT_FAILURE);
}

void
utils_str_toupper(char *const str) {
    char *c;
    c = str;

    while (*c) {
        *c = toupper(*c);
        ++c;
    }
}

uint8_t
utils_swap_byte_nibbles(const uint8_t val) {
    return (val >> 4) | (val << 4);
}