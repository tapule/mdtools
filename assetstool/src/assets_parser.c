/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of mdtools (MegaDrive development tools).
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2025
 * Github: https://github.com/tapule/mdtools
 */

/**
 * \file            assets_parser.c
 * \brief           Assets json script parsing utility
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../libs/pdjson.h"
#include "assets_parser.h"

#define ASSET_PALLETES_KEY  "palettes"
#define ASSET_TILESETS_KEY  "tilesets"
#define ASSET_SHEETS_KEY    "sheets"
#define ASSET_ANIMSS_KEY    "anims"

/**
 * \brief           Show an error message and exit
 * \param[in]       msg: Error message to show (with optionals arguments)
 * \param[in]       ...: Arguments for error message
 */
static void
assets_error(const char *const restrict msg, ...) {
    if (msg != nullptr) {
        va_list args;
        va_start(args);
        vfprintf(stderr, msg, args);
        va_end(args);
        fprintf(stderr, "\n");
    }
    exit(EXIT_FAILURE);
}

static void
asset_parse_palletes(json_stream *const restrict stream, assets_t *const restrict assets)
{
    if (json_next(stream) != JSON_ARRAY) {
        assets_error("Error parsing palletes: Array expected on line %d", json_get_lineno(stream));
    }

    while (json_peek(stream) != JSON_ARRAY_END && json_get_error(stream) == nullptr) {
        asset_desc_t *asset = nullptr;

        if (json_next(stream) != JSON_OBJECT) {
            assets_error("Error parsing palletes: Pallete object expected on line %d", json_get_lineno(stream));
        }

        asset = malloc(sizeof(*asset));
        if (asset == nullptr) {
            assets_error("Error parsing palletes: Not enought memory available");
        }
        memset(asset, 0, sizeof(*asset));

        while (json_peek(stream) != JSON_OBJECT_END && json_get_error(stream) == nullptr) {
            char *key = nullptr;
            const char *value = nullptr;
            if (json_next(stream) != JSON_STRING) {
                assets_error("Error parsing palletes: String expected on line %d", json_get_lineno(stream));
            }

            key = strdup(json_get_string(stream, nullptr));
            if (key == nullptr) {
                assets_error("Error parsing palletes: Not enought memory available");
            }
            if (json_next(stream) != JSON_STRING) {
                assets_error("Error parsing palletes: String expected on line %d", json_get_lineno(stream));
            }
            value = json_get_string(stream, nullptr);

            if (strcmp(key, "name") == 0) {
                asset->name = strdup(value);
                free(key);
                continue;
            }

            if (strcmp(key, "file") == 0) {
                asset->pallete.file = strdup(value);
                free(key);
                continue;
            }

            assets_error("Error parsing palletes: Unknown key '%s' on line %d", key, json_get_lineno(stream));
            free(key);
        }
        /* Validate readed pallete */
        if (asset->name != nullptr && asset->pallete.file != nullptr) {
            asset->next = assets->palletes;
            assets->palletes = asset;
        } else {
            assets_error("Error parsing palletes: Invalid pallete on line %d", json_get_lineno(stream));
        }
        /* Reads the JSON_OBJECT_END */
        json_next(stream);
    }
    /* Reads the JSON_ARRAY_END */
    json_next(stream);
}

static void
asset_parse_tilesets(json_stream *const restrict stream, assets_t *const restrictassets)
{
    if (json_peek(stream) != JSON_ARRAY) {
        assets_error("Error parsing assets: Unknown json file format");
    }
    json_skip(stream);
}

static void
asset_parse_sheets(json_stream *const restrict stream, assets_t *const restrictassets)
{
    if (json_peek(stream) != JSON_ARRAY) {
        assets_error("Error parsing assets: Unknown json file format");
    }
    json_skip(stream);
}

static void
asset_parse_anims(json_stream *const restrict stream, assets_t *const restrictassets)
{
    if (json_peek(stream) != JSON_ARRAY) {
        assets_error("Error parsing assets: Unknown json file format");
    }
    json_skip(stream);
}

void
assets_parse(const char *const restrict path, assets_t *const restrict assets) {
    FILE *json_file = nullptr;
    json_stream stream;

    if (path == nullptr || assets == nullptr) {
        assets_error("Error parsing assets: Invalid arguments");
    }

    memset(assets, 0, sizeof(*assets));

    json_file = fopen(path, "r");
    if (json_file == nullptr) {
        assets_error("Error parsing assets: Can't open file '%s'", path);
    }

    json_open_stream(&stream, json_file);
    if (json_next(&stream) != JSON_OBJECT) {
        assets_error("Error parsing assets: Unknown json file format '%s'", path);
    }

    while (json_peek(&stream) != JSON_OBJECT_END && json_get_error(&stream) == nullptr) {
        const char *key = nullptr;

        if (json_next(&stream) != JSON_STRING) {
            assets_error("Error parsing assets: Unknown json file format '%s'", path);
        }
        key = json_get_string(&stream, nullptr);
        if (strcmp(key, ASSET_PALLETES_KEY) == 0) {
            asset_parse_palletes(&stream, assets);
            continue;
        }

        if (strcmp(key, ASSET_TILESETS_KEY) == 0) {
            asset_parse_tilesets(&stream, assets);
            continue;
        }

        if (strcmp(key, ASSET_SHEETS_KEY) == 0) {
            asset_parse_sheets(&stream, assets);
            continue;
        }

        if (strcmp(key, ASSET_ANIMSS_KEY) == 0) {
            asset_parse_anims(&stream, assets);
            continue;
        }

        assets_error("Error parsing assets: Unknown json string '%s'", key);
    }

    json_close(&stream);
    fclose(json_file);
}