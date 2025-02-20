/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of mdtools (MegaDrive development tools).
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2025
 * Github: https://github.com/tapule/mdtools
 */

/**
 * \file            cli_parser.c
 * \brief           Command line arguments parser utility
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "cli_parser.h"
#include "utils.h"

/**
 * \brief           Assetstool version information
 */
static const char version_text[] =
    "%s v0.01\n"
    "A Sega Megadrive/Genesis assets manager\n"
    "Coded by: Juan Ángel Moreno Fernández (@_tapule) 2025\n"
    "Github: https://github.com/tapule/mdtools\n";

/**
 * \brief           Assetstool usage help
 */
static const char usage_text[] =
    "usage: %s [options]\n"
    "\n"
    "Options:\n"
    "  -v, --version       Show version information and exit\n"
    "  -h, --help          Show this help message and exit\n"
    "  -i <file>           Assets json script input file path\n"
    "                      \"assets.json\" will be used by default\n"
    "  -o <path>           Output path for the generated assets files\n"
    "                      The current directory will be used by default\n"
    "  -d                  Enable debug mode\n"
    "  -dp                 Disable palettes processing\n"
    "  -dt                 Disable tilesets processing\n"
    "  -ds                 Disable sheets processing\n"
    "  -da                 Disable anims processing\n"
    "  -do                 Disable assets optimizations\n"
    "  -dc                 Disable assets compression\n";

/**
 * \brief           Show version information and exit
 * \param[in]       app: App name
 */
static void
cli_version(const char *const restrict app) {
    fprintf(stderr, version_text, app);
    exit(EXIT_SUCCESS);
}

/**
 * \brief           Show usage information plus an optional message and exit
 * \param[in]       app: App name
 * \param[in]       exit_code: Exit code to return to the OS
 * \param[in]       msg: Optional message to show (with optionals arguments)
 * \param[in]       ...: Arguments for message
 */
static void
cli_usage(const char *const restrict app, const int exit_code, const char *const restrict msg, ...) {
    if (msg != nullptr) {
        va_list args;
        va_start(args);
        vfprintf(stderr, msg, args);
        va_end(args);
        fprintf(stderr, "\n\n");
    }
    fprintf(stderr, usage_text, app);
    exit(exit_code);
}

/**
 * \brief           Check and ajust input and output paths as needed
 * \param[in]       app: App name
 * \param[in]       reason: Error message to show (with optionals arguments)
 * \param[in, out]  args: Arguments with paths to check and update
 */
static void
cli_paths_adjust(const char *const restrict app, args_t *const restrict args) {
    struct stat stat_buff;

    /* Set default paths */
    args->input_path = args->input_path != nullptr ? args->input_path : ".";
    args->output_path = args->output_path != nullptr ? args->output_path : ".";

    /* Check and adjust input path and source file name */
    if (stat(args->input_path, &stat_buff) == -1) {
        utils_error("Error: Opening input path '%s'", args->input_path);
    }
    if (S_ISDIR(stat_buff.st_mode)) {
        args->input_file = CLI_DEFAULT_INPUT_FILE;
    } else if (S_ISREG(stat_buff.st_mode)) {
        args->input_file = strrchr(args->input_path, '/');
        if (args->input_file) {
            *args->input_file = '\0';
            ++args->input_file;
        }
    } else {
        cli_usage(app, EXIT_FAILURE, "Error: Unknown input path type '%s'", args->input_path);
    }

    /* Check output path */
    if (stat(args->output_path, &stat_buff) == -1) {
        utils_error("Error: Opening output path '%s'", args->output_path);
    }
    if (S_ISDIR(stat_buff.st_mode) == 0) {
        cli_usage(app, EXIT_FAILURE, "Error: Output path must be a directory '%s'", args->output_path);
    }
}

void
cli_parse(const uint32_t argc, char *const *const argv, args_t *const restrict args) {
    uint32_t i;

    if (argv == nullptr || args == nullptr) {
        cli_usage("assetstool", EXIT_FAILURE, "Error: Invalid arguments");
    }

    memset(args, 0, sizeof(*args));

    i = 1;
    while (i < argc) {
        if ((strcmp(argv[i], "-v") == 0) || (strcmp(argv[i], "--version") == 0)) {
            cli_version(argv[0]);
        } else if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
            cli_usage(argv[0], EXIT_SUCCESS, nullptr);
        }
        /* Input flags parsing */
        else if ((strcmp(argv[i], "-d") == 0) || (strcmp(argv[i], "--debug") == 0)) {
            args->debug_mode = true;
        } else if ((strcmp(argv[i], "-dp") == 0) || (strcmp(argv[i], "--dissable-palettes") == 0)) {
            args->disable_palettes = true;
        } else if ((strcmp(argv[i], "-dt") == 0) || (strcmp(argv[i], "--dissable-tilesets") == 0)) {
            args->disable_tilesets = true;
        } else if ((strcmp(argv[i], "-ds") == 0) || (strcmp(argv[i], "--dissable-sheets") == 0)) {
            args->disable_sheets = true;
        } else if ((strcmp(argv[i], "-da") == 0) || (strcmp(argv[i], "--dissable-anims") == 0)) {
            args->disable_anims = true;
        } else if ((strcmp(argv[i], "-do") == 0) || (strcmp(argv[i], "--dissable-optimizations") == 0)) {
            args->disable_optimizations = true;
        } else if ((strcmp(argv[i], "-dc") == 0) || (strcmp(argv[i], "--dissable-compression") == 0)) {
            args->disable_compression = true;
        }
        /* Input assets json script file path */
        else if (strcmp(argv[i], "-i") == 0) {
            if (i < argc - 1) {
                args->input_path = argv[i + 1];
                ++i;
            } else {
                cli_usage(argv[0], EXIT_FAILURE, "Error: An argument is needed for this option: '%s'", argv[i]);
            }
        }
        /* Output path to save the generated assets files */
        else if (strcmp(argv[i], "-o") == 0) {
            if (i < argc - 1) {
                args->output_path = argv[i + 1];
                ++i;
            } else {
                cli_usage(argv[0], EXIT_FAILURE, "Error: An argument is needed for this option: '%s'", argv[i]);
            }
        } else {
            cli_usage(argv[0], EXIT_FAILURE, "Error: unknown option: '%s'", argv[i]);
        }
        ++i;
    }

    cli_paths_adjust(argv[0], args);
}
