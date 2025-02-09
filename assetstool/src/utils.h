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

#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief           Convert a string to upper case
 * \param[in,out]   str: String to convert
 */
void utils_str_toupper(char *str);

/**
 * \brief           Swap nibbles in a byte
 * \param[in]       val: Original byte to swap
 * \return          Swapped version of value
 */

uint8_t utils_swap_byte_nibbles(const uint8_t val);

#ifdef __cplusplus
}
#endif

#endif /* UTILS_H */