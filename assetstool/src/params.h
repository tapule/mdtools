/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of mdtools (MegaDrive development tools).
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2025
 * Github: https://github.com/tapule/mdtools
 */

/**
 * \file            params.h
 * \brief           Command line parameters parser utility
 */

#ifndef PARAMS_H
#define PARAMS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief           Default assets json script file name
 */
#define DEFAULT_INPUT_FILE "assets.json"

/**
 * \brief           Parameters parsing statuses
 */
typedef enum params_status_t {
    PARAMS_ERROR  = 0,          /**< Parameters processing error */
    PARAMS_STOP,                /**< Processing correct but we must finish */
    PARAMS_CONTINUE             /**< Processing correct, we can continue */
} params_status_t;

/**
 * \brief           Processing flags
 */
typedef enum params_flags_t {
    PARAMS_FLAG_DEBUG                   = (1 << 0), /**< Enable debug mode */
    PARAMS_FLAG_DISABLE_PALLETES        = (1 << 1), /**< Disable palletes processing */
    PARAMS_FLAG_DISABLE_TILESETS        = (1 << 2), /**< Disable tilesets processing */
    PARAMS_FLAG_DISABLE_SHEETS          = (1 << 3), /**< Disable sheets processing */
    PARAMS_FLAG_DISABLE_ANIMS           = (1 << 4), /**< Disable anims processing */
    PARAMS_FLAG_DISABLE_OPTIMIZATIONS   = (1 << 5), /**< Disable assets optimizations */
    PARAMS_FLAG_DISABLE_COMPRESSION     = (1 << 6), /**< Disable assets compression */
} params_flags_t;

/**
 * \brief           Stores processed parameters and flags
 */
typedef struct params_t {
    char *input_file;           /**< Input assets json script file name */
    char *input_path;           /**< Input assets json script file path */
    char *output_path;          /**< Output folder for the generated .h and .c */
    params_flags_t flags;       /**< Processing flags */
} params_t;

/**
 * \brief           Parses the input parameters
 * \param[in]       argc: Input arguments counter
 * \param[in]       argv: Input arguments vector
 * \param[out]      params: Where to store the input processed params
 * \return          PARAMS_ERROR if there was an error
 *                  PARAMS_STOP if the arguments parsing was correct but we must finish (-v or -h)
 *                  PARAMS_CONTINUE if the arguments parsing was correct and we can continue
 */
params_status_t params_parse(const uint32_t argc, char **argv, params_t *restrict params);

/**
 * \brief           Checks and update input and output paths and files
 * \param[in/out]   params: Input processed params to check and update
 * \return          PARAMS_ERROR if there was an error
 *                  PARAMS_CONTINUE if the validation was correct
 */
params_status_t params_validate_paths(params_t *restrict params);

#ifdef __cplusplus
}
#endif

#endif /* PARAMS_H */