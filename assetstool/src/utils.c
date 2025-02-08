/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of mdtools (MegaDrive development tools).
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2025
 * Github: https://github.com/tapule/mdtools
 */

/**
 * \file            utils.h
 * \brief           General utility functions
 */

#include "utils.h"
#include <ctype.h>

void
utils_str_toupper(const char *str) {
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