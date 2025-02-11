/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of mdtools (MegaDrive development tools).
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2025
 * Github: https://github.com/tapule/mdtools
 */

/**
 * \file            cli_parser.h
 * \brief           Command line arguments parser utility
 */

#ifndef CLI_PARSER_H
#define CLI_PARSER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief           Default assets json script file name
 */
#define CLI_DEFAULT_INPUT_FILE "assets.json"

/**
 * \brief           Stores processed arguments and flags
 */
typedef struct args_t {
    char *input_file;           /**< Input assets json script file name */
    char *input_path;           /**< Input assets json script file path */
    char *output_path;          /**< Output folder for the generated .h and .c */
    bool debug_mode;            /**< Debug mode enabled flag */
    bool disable_palletes;      /**< Disable palletes processing flag */
    bool disable_tilesets;      /**< Disable tilesets processing flag */
    bool disable_sheets;        /**< Disable sheets processing flag */
    bool disable_anims;         /**< Disable anims processing flag */
    bool disable_optimizations; /**< Don't do optimizations flag */
    bool disable_compression;   /**< Disable compression flag */
} args_t;

/**
 * \brief           Parses the input arguments
 * \param[in]       argc: Input arguments counter
 * \param[in]       argv: Input arguments vector
 * \param[out]      args: Where to store the input processed arguments
 */
void cli_parse(const uint32_t argc, char *const *const argv, args_t *const restrict args);

#ifdef __cplusplus
}
#endif

#endif /* CLI_PARSER_H */