/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of mdtools (MegaDrive development tools).
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2025
 * Github: https://github.com/tapule/mdtools
 */

/**
 * \file            palettes.c
 * \brief           Palettes processing functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../libs/lodepng.h"
#include "palettes.h"
#include "utils.h"

#define PALETTE_MAX_COLORS          (64)   /* Max colors in a Megadrive palete */
#define PALETTE_MAX_PATH_LENGTH     (1024) /* Max length for palettes paths */
#define PALETTE_MAX_COLORS_PER_LINE (8)    /* Number of color per line to write in source file */

/**
 * \brief           Stores palette's data
 */
typedef struct palette_t {
    asset_desc_t *asset;                 /**< Original palette asset description */
    uint16_t colors[PALETTE_MAX_COLORS]; /**< Palette color storage */
    uint8_t size;                        /**< Palette's size in colors */
} palette_t;

/**
 * \brief           Read a png file and convert its palette to Megadrive format
 * \param[in]       path: Png file path to read
 * \param[in, out]  pal: Palette structure to store readed data
 * \return          true on success, false otherwise
 */
static bool
palette_read(const char *const restrict path, palette_t *const restrict pal) {
    uint32_t error;
    LodePNGState png_state;
    size_t png_size;
    uint8_t *png_data = nullptr;
    uint8_t *img_data = nullptr;
    uint32_t img_width;
    uint32_t img_height;

    /* Load the file into a memory buffer, no png checks here */
    error = lodepng_load_file(&png_data, &png_size, path);
    if (error) {
        free(png_data);
        fprintf(stderr, "Palletes: Error processing %s: %s\n", pal->asset->name, lodepng_error_text(error));
        return false;
    }
    lodepng_state_init(&png_state);
    png_state.decoder.color_convert = false;
    /* Decode our png image and free unneded resources */
    error = lodepng_decode(&img_data, &img_width, &img_height, &png_state, png_data, png_size);
    free(png_data);
    free(img_data);
    /* Checks for errors in the decode stage */
    if (error) {
        fprintf(stderr, "Palletes: Error processing %s: %s\n", pal->asset->name, lodepng_error_text(error));
        return false;
    }
    /* Checks if the image is an indexed one */
    if (png_state.info_raw.colortype != LCT_PALETTE) {
        fprintf(stderr, "Palletes: Error processing %s: Image must be in indexed color mode\n", pal->asset->name);
        return false;
    }

    /* Read a maximum of 64 colors */
    pal->size = png_state.info_png.color.palettesize > PALETTE_MAX_COLORS ? PALETTE_MAX_COLORS
                                                                          : png_state.info_png.color.palettesize;

    /* Do the conversion to a Sega Megadrive/Genesis palette */
    for (uint8_t i = 0; i < pal->size; ++i) {
        uint8_t r_component;
        uint8_t g_component;
        uint8_t b_component;

        /* Read the color components from the png palette */
        r_component = png_state.info_png.color.palette[(i * 4) + 0];
        g_component = png_state.info_png.color.palette[(i * 4) + 1];
        b_component = png_state.info_png.color.palette[(i * 4) + 2];

        /*
            Convert color components to Sega Megadrive/Genesis format:
                000 BBB0 GGG0 RRR0
            9bits: 3bits of blue, 3bits of green, 3bits of red (inverse order)
        */
        r_component = (r_component >> 4) & 0xE;
        g_component = (g_component >> 4) & 0xE;
        b_component = (b_component >> 4) & 0xE;

        pal->colors[i] = (r_component << 0) | (g_component << 4) | (b_component << 8);
    }

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
palettes_build_header(const char *const restrict path, palette_t *const restrict palettes,
                      const uint16_t palettes_count) {
    FILE *h_file;
    char file_path[PALETTE_MAX_PATH_LENGTH];

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
palettes_build_source(const char *const restrict path, palette_t *const restrict palettes,
                      const uint16_t palettes_count) {
    FILE *c_file;
    char file_path[PALETTE_MAX_PATH_LENGTH];

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
    return true;
}

void
palettes_process(args_t *const restrict config, assets_t *const restrict assets) {
    palette_t *palettes = nullptr;
    asset_desc_t *current = nullptr;
    char file_path[PALETTE_MAX_PATH_LENGTH] = {0};

    if (config == nullptr || assets == nullptr || assets->palettes == nullptr) {
        utils_error("Error processing palettes: Invalid arguments");
    }

    palettes = malloc(sizeof(*palettes) * assets->palettes_count);
    if (palettes == nullptr) {
        utils_error("Error processing palettes: Not enought memory available");
    }
    memset(palettes, 0, sizeof(*palettes) * assets->palettes_count);

    current = assets->palettes;
    for (uint16_t i = 0; i < assets->palettes_count; ++i) {
        printf("Palettes: Processing %s\n", current->name);

        /* Builds the complete file path */
        strcpy(file_path, config->input_path);
        strcat(file_path, "/");
        strcat(file_path, current->palette.file);
        palettes[i].asset = current;
        if (!palette_read(file_path, &palettes[i])) {
            free(palettes);
            utils_error("Palettes: Error processing palette %d of %d, stopping", i + 1, assets->palettes_count);
        }
        current = current->next;
    }
    if (!palettes_build_header(config->output_path, palettes, assets->palettes_count)) {
        utils_error("Palettes: Error building %s/palettes.h", config->output_path);
    }
    if (!palettes_build_source(config->output_path, palettes, assets->palettes_count)) {
        utils_error("Palettes: Error building %s/palettes.c", config->output_path);
    }
}
