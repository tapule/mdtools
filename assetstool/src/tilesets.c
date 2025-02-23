/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of mdtools (MegaDrive development tools).
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2025
 * Github: https://github.com/tapule/mdtools
 */

/**
 * \file            tilesets.c
 * \brief           Tilesets processing functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../libs/lodepng.h"
#include "tilesets.h"
#include "utils.h"
#include "img4bpp.h"

#define TILESET_MAX_TILES       (2048) /* Max tiles in a Megadrive tileset */
#define TILESET_MAX_PATH_LENGTH (1024) /* Max length for tilesets paths */

/**
 * \brief           Stores tileset's data
 */
typedef struct tileset_t {
    asset_desc_t *asset; /**< Original tileset asset description */
    uint8_t *tiles;      /**< Tiles storage */
    uint16_t size;       /**< Tileset's size in tiles */
} tileset_t;


/**
 * \brief           Read a png file and convert its palette to Megadrive format
 * \param[in]       path: Png file path to read
 * \param[in, out]  pal: Palette structure to store readed data
 * \return          true on success, false otherwise
 */
static bool
tileset_read(const char *const restrict path, tileset_t *const restrict tileset) {
    uint32_t error;
    LodePNGState png_state;
    size_t png_size;
    uint8_t *png_data = nullptr;
    uint8_t *img_data = nullptr;
    uint8_t *img_4bpp = nullptr;
    uint32_t img_width;
    uint32_t img_height;

    /* Load the file into a memory buffer, no png checks here */
    error = lodepng_load_file(&png_data, &png_size, path);
    if (error) {
        free(png_data);
        fprintf(stderr, "Tilesets: Error processing %s: %s\n", tileset->asset->name, lodepng_error_text(error));
        return false;
    }
    lodepng_state_init(&png_state);
    png_state.decoder.color_convert = false;
    /* Decode our png image and free unneded resources */
    error = lodepng_decode(&img_data, &img_width, &img_height, &png_state, png_data, png_size);
    free(png_data);
    /* Checks for errors in the decode stage */
    if (error) {
        fprintf(stderr, "Tilesets: Error processing %s: %s\n", tileset->asset->name, lodepng_error_text(error));
        return false;
    }
    /* Checks if the image is an indexed one */
    if (png_state.info_raw.colortype != LCT_PALETTE) {
        fprintf(stderr, "Tilesets: Error processing %s: Image must be in indexed color mode\n", tileset->asset->name);
        return false;
    }

    /* Checks if the image is a 4bpp or 8bpp one */
    if (png_state.info_png.color.bitdepth != 4 && png_state.info_png.color.bitdepth != 8) {
        fprintf(stderr, "Tilesets: Error processing %s: Image must be 4bpp or 8bpp\n", tileset->asset->name);
        return false;
    }

    /* Checks if the image has more than 16 colors */
    if (png_state.info_png.color.palettesize > 16) {
        fprintf(stderr, "Tilesets: Error processing %s: Image has more than 16 colors\n", tileset->asset->name);
        return false;
    }

    /* Checks if image width is multiple of 8 */
    if (img_width % 8) {
        fprintf(stderr, "Tilesets: Error processing %s: Image width is not multiple of 8\n", tileset->asset->name);
        return false;
    }

    /* Checks if image height is multiple of 8 */
    if (img_height % 8) {
        fprintf(stderr, "Tilesets: Error processing %s: Image height is not multiple of 8\n", tileset->asset->name);
        return false;
    }

    /* Converts the image to Megadrive 4bpp format if it is 8bpp */
    img_4bpp = img_data;
    if (png_state.info_png.color.bitdepth == 8) {
        img_4bpp = img4bpp_from_8bpp(img_data, img_width * img_width);
        free(img_data);

        if (img_4bpp == nullptr) {
            fprintf(stderr, "Tilesets: Error processing %s: Unable to convert image to 4bpp\n", tileset->asset->name);
            return false;
        }
    }

    /* Extract the tileset from our 4bpp image data */
    tileset->tiles = img4bpp_extract_tiles(img_4bpp, img_width, img_height);
    free(img_4bpp);
    if (tileset->tiles == nullptr) {
        fprintf(stderr, "Tilesets: Error processing %s: Unable to extract tiles from image\n", tileset->asset->name);
        return false;
    }

    /* Set the number of tiles in the tileset */
    tileset->size = (img_width / 8) * (img_height / 8);

    return true;
}

/**
 * \brief           Builds the C header file for the generated palettes
 * \param[in]       path: Destinatio path for the .h file
 * \param[in]       palettes: Palettes array
 * \param[in]       palettes_count: Number of palettes to process
 * \return          true if everythig was correct, false otherwise
 */
static bool
tilesets_build_header(const char *const restrict path, tileset_t *const restrict tilesets,
                      const uint16_t tilesets_count) {
    FILE *h_file;
    char file_path[TILESET_MAX_PATH_LENGTH];
#if 0
    /* Builds the .h complete file path */
    strcpy(file_path, path);
    strcat(file_path, "/");
    strcat(file_path, "palettes.h");

    h_file = fopen(file_path, "w");
    if (!h_file) {
        return false;
    }

    /* Output an information message */
    fprintf(h_file, "/* Generated with assetstool                          */\n");
    fprintf(h_file, "/* a Sega Megadrive/Genesis assets converter          */\n");
    fprintf(h_file, "/* Github: https://github.com/tapule/mdtools          */\n\n");

    /* Header include guard */
    fprintf(h_file, "#ifndef ASSETS_PALETTES_H\n");
    fprintf(h_file, "#define ASSETS_PALETTES_H\n\n");
    fprintf(h_file, "#include <stdint.h>\n\n");

    /* Palette sizes defines */
    for (uint16_t i = 0; i < palettes_count; ++i) {
        char *current_name = nullptr;

        current_name = strdup(palettes[i].asset->name);
        if (!current_name) {
            return false;
        }
        utils_str_toupper(current_name);
        fprintf(h_file, "#define %s_PAL_SIZE    (%d)\n", current_name, palettes[i].size);
        free(current_name);
    }
    fprintf(h_file, "\n");

    /* Palette declarations */
    for (uint16_t i = 0; i < palettes_count; ++i) {
        char *current_name = nullptr;

        current_name = strdup(palettes[i].asset->name);
        if (!current_name) {
            return false;
        }
        utils_str_toupper(current_name);
        fprintf(h_file, "extern const uint16_t %s_pal[%s_PAL_SIZE];\n", palettes[i].asset->name, current_name);
        free(current_name);
    }
    fprintf(h_file, "\n");

    /* End of header include guard */
    fprintf(h_file, "#endif /* ASSETS_PALETTES_H */\n");

    fclose(h_file);
#endif
    return true;
}

/**
 * \brief           Builds the C source file for the generated palettes
 * \param[in]       path: Destinatio path for the .c file
 * \param[in]       palettes: Palettes array
 * \param[in]       palettes_count: Number of palettes to process
 * \return          true if everythig was correct, false otherwise
 */
static bool
tilesets_build_source(const char *const restrict path, tileset_t *const restrict tileset,
                      const uint16_t tilesets_count) {
    FILE *c_file;
    char file_path[TILESET_MAX_PATH_LENGTH];
#if 0
    /* Builds the .c complete file path */
    strcpy(file_path, path);
    strcat(file_path, "/");
    strcat(file_path, "palettes.c");

    c_file = fopen(file_path, "w");
    if (!c_file) {
        return false;
    }

    /* Writes the header include */
    fprintf(c_file, "#include \"palettes.h\"\n\n");

    /* Palette definitions */
    for (uint16_t i = 0; i < palettes_count; ++i) {
        char *current_name = nullptr;

        current_name = strdup(palettes[i].asset->name);
        if (!current_name) {
            return false;
        }

        utils_str_toupper(current_name);
        fprintf(c_file, "const uint16_t %s_pal[%s_PAL_SIZE] = {", palettes[i].asset->name, current_name);
        /* Writes palette color values */
        for (uint8_t j = 0; j < palettes[i].size; ++j) {
            /* Do we need to write a comma after the last written value? */
            if (j > 0) {
                fprintf(c_file, ", ");
            }
            /* Every X written colors, add a line feed */
            if (j % PALETTE_MAX_COLORS_PER_LINE == 0) {
                fprintf(c_file, "\n    ");
            }
            fprintf(c_file, "0x%04X", palettes[i].colors[j]);
        }
        fprintf(c_file, "\n};\n\n");
        free(current_name);
    }

    fclose(c_file);
#endif
    return true;
}

static void
tilesets_free(tileset_t *const restrict tilesets, const uint16_t tilesets_count) {
    for (uint16_t i = 0; i < tilesets_count; ++i) {
        if (tilesets[i].tiles != nullptr) {
            free(tilesets[i].tiles);
        }
    }
}

void
tilesets_process(args_t *const restrict config, assets_t *const restrict assets) {
    tileset_t *tilesets = nullptr;
    asset_desc_t *current = nullptr;
    char file_path[TILESET_MAX_PATH_LENGTH] = {0};

    if (config == nullptr || assets == nullptr || assets->tilesets == nullptr) {
        utils_error("Error processing tilesets: Invalid arguments");
    }

    tilesets = malloc(sizeof(*tilesets) * assets->tilesets_count);
    if (tilesets == nullptr) {
        utils_error("Error processing tilesets: Not enought memory available");
    }
    memset(tilesets, 0, sizeof(*tilesets) * assets->tilesets_count);

    current = assets->tilesets;
    for (uint16_t i = 0; i < assets->tilesets_count; ++i) {
        printf("Tilesets: Processing %s\n", current->name);

        /* Builds the complete file path */
        strcpy(file_path, config->input_path);
        strcat(file_path, "/");
        strcat(file_path, current->tileset.file);
        tilesets[i].asset = current;
        if (!tileset_read(file_path, &tilesets[i])) {
            tilesets_free(tilesets, assets->tilesets_count);
            utils_error("Tilesets: Error processing tileset %d of %d, stopping", i + 1, assets->tilesets_count);
        }
        current = current->next;
    }
    if (!tilesets_build_header(config->output_path, tilesets, assets->tilesets_count)) {
        tilesets_free(tilesets, assets->tilesets_count);
        utils_error("Tilesets: Error building %s/tilesets.h", config->output_path);
    }
    if (!tilesets_build_source(config->output_path, tilesets, assets->tilesets_count)) {
        tilesets_free(tilesets, assets->tilesets_count);
        utils_error("Tilesets: Error building %s/tilesets.c", config->output_path);
    }
    tilesets_free(tilesets, assets->tilesets_count);
}
