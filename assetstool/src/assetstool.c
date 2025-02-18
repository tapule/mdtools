/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of mdtools (MegaDrive development tools).
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2025
 * Github: https://github.com/tapule/mdtools
 */

/**
 * \file            assetstool.h
 * \brief           TODO
 *
 * A Sega Megadrive/Genesis assets manager
 *
 * Extracts Sega Megadrive/Genesis ...
 */

#include <dirent.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "../libs/lodepng.h"
#include "utils.h"
#include "cli_parser.h"
#include "assets_parser.h"

#define MAX_TILESETS         512    /* Enough?? */
#define MAX_FILE_NAME_LENGTH 128    /* Max length for file names */
#define MAX_PATH_LENGTH      1024   /* Max length for paths */

int
main(int argc, char **argv) {
    args_t config = {0};
    assets_t assets;
    char assets_file[1024] = {0};

    /* Argument reading and processing */
    cli_parse(argc, argv, &config);

    /* Asset json script file parsing */
    fprintf(stdout, "Parsing assets file...\n");
    strcpy(assets_file, config.input_path);
    strcat(assets_file, "/");
    strcat(assets_file, config.input_file);
    assets_parse(assets_file, &assets);

    fprintf(stdout, "Processing assets...\n");
    if (!config.disable_palettes && assets.palettes != nullptr) {
        fprintf(stdout, "Palettes: Processing...\n");

    } else {
        fprintf(stdout, "Paletes processing disabled\n");
    }

#if 0
    /* First try to open source path as a directory */
    dir = opendir(params.src_path);
    if (dir != NULL) {
        printf(version_text);
        printf("\nReading files...\n");
        while ((dir_entry = readdir(dir)) != NULL) {
            /* Checks max allowed tilesets */
            if (tileset_index >= MAX_TILESETS) {
                closedir(dir);
                fprintf(stderr, "Error: More than %d files in the source directory\n", MAX_TILESETS);
                return EXIT_FAILURE;
            }

            /* Process only regular files */
            if (dir_entry->d_type == DT_REG) {
                if (!tileset_read(params.src_path, dir_entry->d_name, tileset_index)) {
                    printf("\tPng file to tiles: %s -> %s\n", dir_entry->d_name, tilesets[tileset_index].name);
                    ++tileset_index;
                }
            }
        }
        closedir(dir);
    }
    /* We can't open source path as directory, try to open it as file instead */
    else {
        /* Get the file name and path */
        file_name = strrchr(params.src_path, '/');
        if (file_name) {
            *file_name = '\0';
            ++file_name;
        } else {
            file_name = params.src_path;
            params.src_path = ".";
        }
        printf(version_text);
        printf("\nReading file...\n");
        if (!tileset_read(params.src_path, file_name, tileset_index)) {
            printf("\tFile to binary: %s -> %s\n", file_name, tilesets[tileset_index].name);
            ++tileset_index;
        }
    }

    printf("%d tilesets readed.\n", tileset_index);

    if (tileset_index > 0) {
        /* By default use BASE_NAME as prefix for files, defines, vars, etc */
        bool use_prefix = true;

        /* Adjust the destination base name if it was not specified */
        if (!params.dest_name) {
            /* Only one file, use its name as base name and no prefix */
            if (tileset_index == 1) {
                params.dest_name = tilesets[0].name;
                use_prefix = false;
            }
            /* More than one file, use "bins" as base name */
            else {
                params.dest_name = "til";
            }
        }

        printf("Building C header file...\n");
        build_header_file(params.dest_path, params.dest_name, use_prefix, tileset_index);
        printf("Building C source file...\n");
        build_source_file(params.dest_path, params.dest_name, use_prefix, tileset_index);
        printf("Done.\n");
    }
#endif
    return EXIT_SUCCESS;
}
