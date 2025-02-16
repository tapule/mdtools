/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of mdtools (MegaDrive development tools).
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2025
 * Github: https://github.com/tapule/mdtools
 */

/**
 * \file            assets_parser.h
 * \brief           Assets json script parsing utility
 */

#ifndef ASSETS_PARSER_H
#define ASSETS_PARSER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum asset_compression_t {
    ASSET_COMPRESSION_NONE = 0,
    ASSET_COMPRESSION_ZX0,
    ASSET_COMPRESSION_SLZ
} asset_compression_t;

typedef enum asset_sheet_type_t {
    ASSET_SHEET_TYPE_STATIC = 0,
    ASSET_SHEET_TYPE_DYNAMIC
} asset_sheet_type_t;

typedef enum asset_sheet_layout_t {
    ASSET_SHEET_LAYOUT_SPRITE = 0,
    ASSET_SHEET_LAYOUT_TILEMAP
} asset_sheet_layout_t;

typedef struct asset_subframe_t asset_subframe_t;

struct asset_subframe_t {
    uint8_t x;
    uint8_t y;
    uint8_t w;
    uint8_t h;
    asset_subframe_t *next;
};

typedef struct asset_frame_t asset_frame_t;

struct asset_frame_t {
    asset_subframe_t *subframes; // Linked list
    asset_frame_t *next;
};

typedef struct asset_sequence_idx_t asset_sequence_idx_t;

struct asset_sequence_idx_t {
    uint8_t idx;
    asset_sequence_idx_t *next;
};

typedef struct asset_desc_t asset_desc_t;

struct asset_desc_t {
    uint16_t status;
    char *name;

    union {
        struct {
            char *file;
        } pallete;

        struct {
            char *file;
            asset_compression_t compression;
        } tileset;

        struct {
            char *file;
            asset_compression_t compression;
            asset_sheet_type_t type;
            asset_sheet_layout_t layout;
            uint16_t frame_w;
            uint16_t frame_h;
            asset_frame_t *frames; // Linked list
        } sheet;

        struct {
            char *sheet;

            struct {
                uint8_t x;
                uint8_t y;
            } pivot;

            struct {
                bool loop;
                bool inverted;
                bool random;
                bool hide;
            } properties;

            uint8_t rate;
            uint8_t delay;
            asset_sequence_idx_t *sequence;
        } anim;
    };

    asset_desc_t *next;
};

typedef struct assets_t {
    asset_desc_t *palletes;
    asset_desc_t *tilesets;
    asset_desc_t *sheets;
    asset_desc_t *anims;
} assets_t;

/**
 * \brief           Parse
 * \param[in,out]
 */
void assets_parse(const char *const restrict path, assets_t *const restrict assets);

#ifdef __cplusplus
}
#endif

#endif /* ASSETS_PARSER_H */