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
#include <string.h>

static const char version_text[] =
    "assetstool v0.01\n"
    "A Sega Megadrive/Genesis assets manager\n"
    "Coded by: Juan Ángel Moreno Fernández (@_tapule) 2025\n"
    "Github: https://github.com/tapule/mdtools\n";

static const char help_text[] =
    "usage: assetstool [options]\n"
    "\n"
    "Options:\n"
    "  -v, --version       Show version information and exit\n"
    "  -h, --help          Show this help message and exit\n"
    "  -i <file>           Use a directory path to look for png files\n"
    "                      or a unique png file to extract tiles from"
    "                      Current directory will be used as default\n"
    "  -o <path>           Use a path to save generated C source files\n"
    "                      The current directory will be used as default\n";

params_status_t
params_parse(const uint32_t argc, char **argv, params_t *restrict params) {
    uint32_t i;

    i = 1;
    while (i < argc) {
        if ((strcmp(argv[i], "-v") == 0) || (strcmp(argv[i], "--version") == 0)) {
            fputs(version_text, stdout);
            return PARAMS_STOP;
        } else if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
            fputs(help_text, stdout);
            return PARAMS_STOP;
        }
        /* Input json assets script file path */
        else if (strcmp(argv[i], "-i") == 0) {
            if (i < argc - 1) {
                params->input_path = argv[i + 1];
                ++i;
            } else {
                fprintf(stderr, "%s: an argument is needed for this option: '%s'\n", argv[0], argv[i]);
                return PARAMS_ERROR;
            }
        }
        /* Output path to save the generated .h and .c files */
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
