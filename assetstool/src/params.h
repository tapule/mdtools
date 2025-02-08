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
 * \brief           Parameters parsing statuses
 */
typedef enum params_status_t {
    PARAMS_ERROR  = 0,          /**< Parameters processing error */
    PARAMS_STOP,                /**< Processing correct but we must finish */
    PARAMS_CONTINUE             /**< Processing correct, we can continue */
} params_status_t;

/**
 * \brief           Stores the processed parameters
 */
typedef struct params_t {
    char *input_path;           /* Input assets json script file path */
    char *output_path;          /* Output folder for the generated .h and .c */
    char *dest_name;            /* Base name for the generated .h and .c files */
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

#ifdef __cplusplus
}
#endif

#endif /* PARAMS_H */