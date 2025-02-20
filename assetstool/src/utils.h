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
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief           Some ANSI terminal color codes to use with printf functions
 */
#define PRINT_COLOR_RESET      "\033[0m"
#define PRINT_COLOR_RED        "\033[1;31;49m"
#define PRINT_COLOR_GREEN      "\033[1;32;49m"
#define PRINT_COLOR_YELLOW     "\033[1;33;49m"
#define PRINT_COLOR_BLUE       "\033[1;34;49m"
#define PRINT_COLOR_MAGENTA    "\033[1;35;49m"
#define PRINT_COLOR_CYAN       "\033[1;36;49m"
#define PRINT_COLOR_WHITE      "\033[1;37;49m"

/**
 * \brief           Set output terminal writing color
 * \param[in]       strem: Output file strem for writing operations
 * \param[in]       color: ANSI color code string
 */
void fprintf_color_set(FILE *const restrict stream, const char *const restrict color);

/**
 * \brief           Reset the output terminal writing color
 * \param[in]       strem: Output file strem for writing operations
 */
void fprintf_color_reset(FILE *const restrict stream);

/**
 * \brief           Show an error message and exit
 * \param[in]       msg: Error message to show (with optionals arguments)
 * \param[in]       ...: Arguments for error message
 */
void utils_error(const char *const restrict msg, ...);

/**
 * \brief           Convert a string to upper case
 * \param[in,out]   str: String to convert
 */
void utils_str_toupper(char *const str);

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