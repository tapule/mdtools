/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of mdtools (MegaDrive development tools).
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2025
 * Github: https://github.com/tapule/mdtools
 */

/**
 * \file            params.c
 * \brief           Command line parameters parser utility
 */

#include "params.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/**
 * \brief           Assetstool version information
 */
static const char version_text[] =
    "assetstool v0.01\n"
    "A Sega Megadrive/Genesis assets manager\n"
    "Coded by: Juan Ángel Moreno Fernández (@_tapule) 2025\n"
    "Github: https://github.com/tapule/mdtools\n";

/**
 * \brief           Assetstool usage help
 */
static const char help_text[] =
    "usage: assetstool [options]\n"
    "\n"
    "Options:\n"
    "  -v, --version       Show version information and exit\n"
    "  -h, --help          Show this help message and exit\n"
    "  -i <file>           Assets json script input file path\n"
    "                      \"assets.json\" will be used by default\n"
    "  -o <path>           Output path for the generated assets files\n"
    "                      The current directory will be used by default\n"
    "  -d                  Enable debug mode\n"
    "  -dp                 Disable palletes processing\n"
    "  -dt                 Disable tilesets processing\n"
    "  -ds                 Disable sheets processing\n"
    "  -da                 Disable anims processing\n"
    "  -do                 Disable assets optimizations\n"
    "  -dc                 Disable assets compression\n";

params_status_t
params_parse(const uint32_t argc, char **argv, params_t *restrict params) {
    uint32_t i;

    if (argv == nullptr || params == nullptr) {
        fprintf(stderr, "Error: Invalid arguments\n");
        return PARAMS_ERROR;
    }

    i = 1;
    while (i < argc) {
        if ((strcmp(argv[i], "-v") == 0) || (strcmp(argv[i], "--version") == 0)) {
            fputs(version_text, stdout);
            return PARAMS_STOP;
        } else if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
            fputs(help_text, stdout);
            return PARAMS_STOP;
        }
        /* Input flags parsing */
        else if ((strcmp(argv[i], "-d") == 0) || (strcmp(argv[i], "--debug") == 0)) {
            params->flags |= PARAMS_FLAG_DEBUG;
        } else if ((strcmp(argv[i], "-dp") == 0) || (strcmp(argv[i], "--dissable-palletes") == 0)) {
            params->flags |= PARAMS_FLAG_DISABLE_PALLETES;
        } else if ((strcmp(argv[i], "-dt") == 0) || (strcmp(argv[i], "--dissable-tilesets") == 0)) {
            params->flags |= PARAMS_FLAG_DISABLE_TILESETS;
        } else if ((strcmp(argv[i], "-ds") == 0) || (strcmp(argv[i], "--dissable-sheets") == 0)) {
            params->flags |= PARAMS_FLAG_DISABLE_SHEETS;
        } else if ((strcmp(argv[i], "-da") == 0) || (strcmp(argv[i], "--dissable-anims") == 0)) {
            params->flags |= PARAMS_FLAG_DISABLE_ANIMS;
        } else if ((strcmp(argv[i], "-do") == 0) || (strcmp(argv[i], "--dissable-optimizations") == 0)) {
            params->flags |= PARAMS_FLAG_DISABLE_OPTIMIZATIONS;
        } else if ((strcmp(argv[i], "-dc") == 0) || (strcmp(argv[i], "--dissable-compression") == 0)) {
            params->flags |= PARAMS_FLAG_DISABLE_COMPRESSION;
        }
        /* Input assets json script file path */
        else if (strcmp(argv[i], "-i") == 0) {
            if (i < argc - 1) {
                params->input_path = argv[i + 1];
                ++i;
            } else {
                fprintf(stderr, "%s: an argument is needed for this option: '%s'\n", argv[0], argv[i]);
                return PARAMS_ERROR;
            }
        }
        /* Output path to save the generated assets files */
        else if (strcmp(argv[i], "-o") == 0) {
            if (i < argc - 1) {
                params->output_path = argv[i + 1];
                ++i;
            } else {
                fprintf(stderr, "%s: an argument is needed for this option: '%s'\n", argv[0], argv[i]);
                return PARAMS_ERROR;
            }
        } else {
            fprintf(stderr, "%s: unknown option: '%s'\n", argv[0], argv[i]);
            return PARAMS_ERROR;
        }
        ++i;
    }
    return PARAMS_CONTINUE;
}

params_status_t
params_validate_paths(params_t *restrict params) {
    struct stat stat_buff;

    if (params == nullptr) {
        fprintf(stderr, "Error: Invalid parameters\n");
        return PARAMS_ERROR;
    }

    /* Set default paths */
    params->input_path = params->input_path != nullptr ? params->input_path : ".";
    params->output_path = params->output_path != nullptr ? params->output_path : ".";

    /* Check and adjust input path and source file name */
    if (stat(params->input_path, &stat_buff) == -1) {
        fprintf(stderr, "Error: Opening input path '%s'\n", params->input_path);
        return PARAMS_ERROR;
    }
    if (S_ISDIR(stat_buff.st_mode)) {
        params->input_file = DEFAULT_INPUT_FILE;
    } else if (S_ISREG(stat_buff.st_mode)) {
        params->input_file = strrchr(params->input_path, '/');
        if (params->input_file)
        {
            *params->input_file = '\0';
            ++params->input_file;
        }
    } else {
        fprintf(stderr, "Error: Unknown input path type '%s'\n", params->input_path);
        return PARAMS_ERROR;
    }

    /* Check output path */
    if (stat(params->output_path, &stat_buff) == -1) {
        fprintf(stderr, "Error: Opening output path '%s'\n", params->output_path);
        return PARAMS_ERROR;
    }
    if (S_ISDIR(stat_buff.st_mode) == 0) {
        fprintf(stderr, "Error: Output path must be a directory '%s'\n", params->output_path);
        return PARAMS_ERROR;
    }

    return PARAMS_CONTINUE;
}