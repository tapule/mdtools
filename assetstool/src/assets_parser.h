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

/**
 * \brief           Available assets compression types
 */
typedef enum asset_compression_t {
    ASSET_COMPRESSION_NONE = 0, /**< No compression */
    ASSET_COMPRESSION_ZX0,      /**< ZX0/Salvador compression */
    ASSET_COMPRESSION_SLZ       /**< Sik's SLZ compression */
} asset_compression_t;


/**
 * \brief           Sheet types based on vram management used to store their frames
 */
typedef enum asset_sheet_type_t {
    ASSET_SHEET_TYPE_STATIC = 0,    /**< All frames loaded to vram at once */
    ASSET_SHEET_TYPE_DYNAMIC        /**< Frame by frame load to vram */
} asset_sheet_type_t;

/**
 * \brief           Sheet layout types based on the kind of resource (sprite or tilemap)
 */
typedef enum asset_sheet_layout_t {
    ASSET_SHEET_LAYOUT_SPRITE = 0,  /**< Tiles are stored in sprite order */
    ASSET_SHEET_LAYOUT_TILEMAP      /**< Tiles are stored in tilemap order */
} asset_sheet_layout_t;


/**
 * \brief           Subframe object list structure
 */
typedef struct asset_subframe_t asset_subframe_t;
struct asset_subframe_t {
    uint8_t x;
    uint8_t y;
    uint8_t w;
    uint8_t h;
    asset_subframe_t *next;
};

/**
 * \brief           Frame object list structure
 */
typedef struct asset_frame_t asset_frame_t;
struct asset_frame_t {
    asset_subframe_t *subframes; // Linked list
    asset_frame_t *next;
};

/**
 * \brief           Anim sequence index list structure
 */
typedef struct asset_sequence_idx_t asset_sequence_idx_t;
struct asset_sequence_idx_t {
    uint8_t idx;
    asset_sequence_idx_t *next;
};

/**
 * \brief           Asset description structure
 */
typedef struct asset_desc_t asset_desc_t;
struct asset_desc_t {
    char *name;                                 /**< Asset name */

    union {
        /* Palette description structure */
        struct {
            char *file;                         /**< Palette file path */
        } palette;

        /* Tileset description structure */
        struct {
            char *file;                         /**< Tileset file path */
            asset_compression_t compression;    /**< Tileset compression type */
        } tileset;

        /* Sheet description structure */
        struct {
            char *file;                         /**< Sheet file path */
            asset_compression_t compression;    /**< Sheet compression type */
            asset_sheet_type_t type;            /**< Sheet vram management type */
            asset_sheet_layout_t layout;        /**< Sheet tiles storing layout */
            uint16_t frame_w;                   /**< Sheet frames width */
            uint16_t frame_h;                   /**< Sheet frames height */
            asset_frame_t *frames;              /**< Linked list of frames */
        } sheet;

        /* Anim description structure */
        struct {
            char *sheet;                        /**< Anim reference sheet name */

            struct {
                uint8_t x;                      /**< Anim pivot X coordinate */
                uint8_t y;                      /**< Anim pivot Y coordinate */
            } pivot;

            struct {
                bool loop;                      /**< Anim looped property */
                bool inverted;                  /**< Anim inverted playing property */
                bool random;                    /**< Anim random init position property */
                bool hide;                      /**< Anim hide on finish property */
            } properties;

            uint8_t rate;                       /**< Anim speed in ticks */
            uint8_t delay;                      /**< Anim starting delay in ticks */
            asset_sequence_idx_t *sequence;     /**< Sequence indexes linked list */
        } anim;
    };
    asset_desc_t *next;                         /**< Assets linked list */
};


typedef struct assets_t {
    asset_desc_t *palettes;
    asset_desc_t *tilesets;
    asset_desc_t *sheets;
    asset_desc_t *anims;
} assets_t;

/**
 * \brief           Parse and extract assets descriptions from a json file
 *
 * File must be an assets json file with this global structure:
 *  {
 *      "palettes": [
 *      ],
 *      "tilesets": [
 *      ],
 *      "sheets": [
 *      ],
 *      "anims": [
 *      ]
 *  }
 *
 * \param[in, out]  path: Assets json file path
 * \param[in, out]  assets: Assets structure containing parsed assets
 */
void assets_parse(const char *const restrict path, assets_t *const restrict assets);

#ifdef __cplusplus
}
#endif

#endif /* ASSETS_PARSER_H */