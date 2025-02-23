/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of mdtools (MegaDrive development tools).
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2025
 * Github: https://github.com/tapule/mdtools
 */

/**
 * \file            img4bpp.h
 * \brief           4bpp image utilities
 */
#include <stdlib.h>

#include "img4bpp.h"

uint8_t *
img4bpp_from_8bpp(uint8_t *const image, const uint32_t size) {
    uint8_t *img_4bpp = nullptr;
    uint8_t *src = nullptr;
    uint8_t *dest = nullptr;

    /* Requests memory for the new image which needs half the original */
    img_4bpp = malloc(size / 2);
    if (img_4bpp == nullptr) {
        return nullptr;
    }
    src = image;
    dest = img_4bpp;

    /* Converts two bytes in 8bpp (2 pixels) to one byte in 4bpp */
    for (uint32_t i = 0; i < size / 2; ++i) {
        *dest = ((src[0] & 0x0F) << 4) | ((src[1] & 0x0F) << 0);
        ++dest;
        src += 2;
    }

    return img_4bpp;
}

uint8_t *
img4bpp_extract_tiles(uint8_t *const image, const uint32_t width, const uint32_t height) {
    uint32_t tile_width;  /* Width in tiles of our image */
    uint32_t tile_height; /* Height in tiles of our image */
    uint8_t *tiles;       /* Memory storage for our tiles */
    uint8_t *tiles_p;     /* Current position in the tiles memory */
    uint8_t *image_p;     /* Current position in the image memory */
    uint32_t pitch;       /* Jump to the next tile row */
    uint32_t tile_x;      /* Tile x positon counter */
    uint32_t tile_y;      /* Tile y position counter */
    uint32_t tile_row;    /* Row copy position counter */

    /* Image dimesions are in pixels, convert to tiles */
    tile_width = width / 8;
    tile_height = height / 8;

    /*
     A tile is 32 bytes, 8 rows of 4 bytes each. Pitch is the jump in bytes in
     the original image to point to the start of the next row in a tile
    */
    pitch = tile_width * 4;

    /* Requests 32 bytes of memory for each tile */
    tiles = malloc(tile_width * tile_height * 32);
    if (!tiles) {
        return nullptr;
    }

    /* Point to the start of tiles buffer */
    tiles_p = tiles;

    for (tile_y = 0; tile_y < tile_height; ++tile_y) {
        for (tile_x = 0; tile_x < tile_width; ++tile_x) {
            /* Move the image pointer to the start of next tile to process */
            image_p = &image[((tile_y * 8) * pitch) + (tile_x * 4)];

            /* Put current tile's rows in the tiles buffer */
            for (tile_row = 0; tile_row < 8; ++tile_row) {
                tiles_p[0] = image_p[0];
                tiles_p[1] = image_p[1];
                tiles_p[2] = image_p[2];
                tiles_p[3] = image_p[3];
                /* Jump to next tile row */
                image_p += pitch;
                tiles_p += 4;
            }
        }
    }
    return tiles;
}
