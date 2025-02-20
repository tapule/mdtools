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

#include "assets_parser.h"
#include "utils.h"
#include "../libs/pdjson.h"

/**
 * \brief           Parse and extract palette objects from a json stream
 *
 * Stream must point to an array of palette objects and can contain any number
 * of them. Palette object structure is as follows:
 *  {
 *      "name": "palette name",
 *      "file": "palette/png/file_path"
 *  }
 *
 * \param[in, out]  stream: Json stream pointing to an array of objects
 * \param[in, out]  assets: Assets structure to store readed palettes
 */
static void
asset_parse_palettes(json_stream *const restrict stream, assets_t *const restrict assets) {
    asset_desc_t *tail = nullptr;

    /* Stream is at the beginning of an array of palette objects */
    if (json_next(stream) != JSON_ARRAY) {
        utils_error("Error parsing palettes: Array expected on line %d", json_get_lineno(stream));
    }

    while (json_peek(stream) != JSON_ARRAY_END && json_get_error(stream) == nullptr) {
        asset_desc_t *asset = nullptr;

        if (json_next(stream) != JSON_OBJECT) {
            utils_error("Error parsing palettes: Palette object expected on line %d", json_get_lineno(stream));
        }

        asset = malloc(sizeof(*asset));
        if (asset == nullptr) {
            utils_error("Error parsing palettes: Not enought memory available");
        }
        memset(asset, 0, sizeof(*asset));

        ++assets->palettes_count;
        while (json_peek(stream) != JSON_OBJECT_END && json_get_error(stream) == nullptr) {
            char key[32] = {0};
            const char *value = nullptr;

            if (json_next(stream) != JSON_STRING) {
                utils_error("Error parsing palettes: String expected on line %d", json_get_lineno(stream));
            }
            /* We need to keep a copy of current json token string or we'll lost it in the next json_next call */
            strncpy(key, json_get_string(stream, nullptr), 32);
            if (json_next(stream) != JSON_STRING) {
                utils_error("Error parsing palettes: String expected on line %d", json_get_lineno(stream));
            }
            value = json_get_string(stream, nullptr);

            /* Check palette fields */
            if (strcmp(key, "name") == 0) {
                asset->name = strdup(value);
                continue;
            }

            if (strcmp(key, "file") == 0) {
                asset->palette.file = strdup(value);
                continue;
            }
            utils_error("Error parsing palettes: Unknown key '%s' on line %d", key, json_get_lineno(stream));
        }
        /* Validate readed palette */
        if (asset->name != nullptr && asset->palette.file != nullptr) {
            /* Keep list ordered adding new elements at the end */
            if (assets->palettes == nullptr) {
                assets->palettes = asset;
                tail = asset;
            } else {
                tail->next = asset;
                tail = asset;
            }
        } else {
            utils_error("Error parsing palettes: Invalid palette on line %d", json_get_lineno(stream));
        }
        /* Reads the JSON_OBJECT_END */
        json_next(stream);
    }
    /* Reads the JSON_ARRAY_END */
    json_next(stream);
}

/**
 * \brief           Parse and extract tilesets objects from a json stream
 *
 * Stream must point to an array of tileset objects and can contain any number
 * of them. Tileset object structure is as follows:
 *  {
 *      "name": "tileset name",
 *      "file": "tileset/png/file_path",
 *      "compression": "none/zx0/slz"
 *  }
 *
 * \param[in, out]  stream: Json stream pointing to an array of objects
 * \param[in, out]  assets: Assets structure to store readed tilesets
 */
static void
asset_parse_tilesets(json_stream *const restrict stream, assets_t *const restrict assets) {
    asset_desc_t *tail = nullptr;

    if (json_next(stream) != JSON_ARRAY) {
        utils_error("Error parsing tilesets: Array expected on line %d", json_get_lineno(stream));
    }

    while (json_peek(stream) != JSON_ARRAY_END && json_get_error(stream) == nullptr) {
        asset_desc_t *asset = nullptr;

        if (json_next(stream) != JSON_OBJECT) {
            utils_error("Error parsing tilesets: Tileset object expected on line %d", json_get_lineno(stream));
        }

        asset = malloc(sizeof(*asset));
        if (asset == nullptr) {
            utils_error("Error parsing tilesets: Not enought memory available");
        }
        memset(asset, 0, sizeof(*asset));

        ++assets->tilesets_count;
        while (json_peek(stream) != JSON_OBJECT_END && json_get_error(stream) == nullptr) {
            char key[32] = {0};
            const char *value = nullptr;

            if (json_next(stream) != JSON_STRING) {
                utils_error("Error parsing tilesets: String expected on line %d", json_get_lineno(stream));
            }

            strncpy(key, json_get_string(stream, nullptr), 32);
            if (json_next(stream) != JSON_STRING) {
                utils_error("Error parsing tilesets: String expected on line %d", json_get_lineno(stream));
            }
            value = json_get_string(stream, nullptr);

            /* Check tileset fields */
            if (strcmp(key, "name") == 0) {
                asset->name = strdup(value);
                continue;
            }

            if (strcmp(key, "file") == 0) {
                asset->tileset.file = strdup(value);
                continue;
            }

            if (strcmp(key, "compression") == 0) {
                if (strcmp(value, "none") == 0) {
                    asset->tileset.compression = ASSET_COMPRESSION_NONE;
                    continue;
                }

                if (strcmp(value, "zx0") == 0) {
                    asset->tileset.compression = ASSET_COMPRESSION_ZX0;
                    continue;
                }

                if (strcmp(value, "slz") == 0) {
                    asset->tileset.compression = ASSET_COMPRESSION_SLZ;
                    continue;
                }
                utils_error("Error parsing tilesets: Unknown compression type '%s' on line %d", value,
                             json_get_lineno(stream));
            }
            utils_error("Error parsing tilesets: Unknown key '%s' on line %d", key, json_get_lineno(stream));
        }
        /* Validate readed tileset */
        if (asset->name != nullptr && asset->tileset.file != nullptr) {
            if (assets->tilesets == nullptr) {
                assets->tilesets = asset;
                tail = asset;
            } else {
                tail->next = asset;
                tail = asset;
            }
        } else {
            utils_error("Error parsing tilesets: Invalid tileset on line %d", json_get_lineno(stream));
        }
        /* Reads the JSON_OBJECT_END */
        json_next(stream);
    }
    /* Reads the JSON_ARRAY_END */
    json_next(stream);
}

/**
 * \brief           Parse and extract subframe objects from a json stream
 *
 * Stream must point to an array of subframe objects and can contain any number
 * of them. Subframe object structure is as follows:
 *  {
 *      "x": [0..255]
 *      "y": [0..255]
 *      "w": [1..4]
 *      "h": [1..4]
 *  }
 *
 * \param[in, out]  stream: Json stream pointing to an array of objects
 * \param[in, out]  frame: Frame structure to store readed subframes
 */
static void
asset_parse_subframes(json_stream *const restrict stream, asset_frame_t *const restrict frame) {
    asset_subframe_t *tail = nullptr;

    if (json_next(stream) != JSON_ARRAY) {
        utils_error("Error parsing subframes: Unknown json file format");
    }

    while (json_peek(stream) != JSON_ARRAY_END && json_get_error(stream) == nullptr) {
        asset_subframe_t *subframe = nullptr;

        if (json_next(stream) != JSON_OBJECT) {
            utils_error("Error parsing subframes: Subframe object expected on line %d", json_get_lineno(stream));
        }

        subframe = malloc(sizeof(*subframe));
        if (subframe == nullptr) {
            utils_error("Error parsing subframe: Not enought memory available");
        }
        memset(subframe, 0, sizeof(*subframe));

        ++frame->subframes_count;
        while (json_peek(stream) != JSON_OBJECT_END && json_get_error(stream) == nullptr) {
            char key[32] = {0};

            /* Item name string */
            if (json_next(stream) != JSON_STRING) {
                utils_error("Error parsing frames: String expected on line %d", json_get_lineno(stream));
            }
            strncpy(key, json_get_string(stream, nullptr), 32);

            /* Item value stored in the json stream state */
            if (json_next(stream) != JSON_NUMBER) {
                utils_error("Error parsing subframe: Number expected on line %d", json_get_lineno(stream));
            }

            /* Check subframe fields */
            if (strcmp(key, "x") == 0) {
                subframe->x = (uint8_t) json_get_number(stream);
                continue;
            }
            if (strcmp(key, "y") == 0) {
                subframe->y = (uint8_t) json_get_number(stream);
                continue;
            }
            if (strcmp(key, "w") == 0) {
                subframe->w = (uint8_t) json_get_number(stream);
                continue;
            }
            if (strcmp(key, "h") == 0) {
                subframe->h = (uint8_t) json_get_number(stream);
                continue;
            }
            utils_error("Error parsing subframe: Unknown key '%s' on line %d", key, json_get_lineno(stream));
        }
        /* Validate readed subframe */
        if (subframe->w > 0 && subframe->w < 5 && subframe->h > 0 && subframe->h < 5) {
            if (frame->subframes == nullptr) {
                frame->subframes = subframe;
                tail = subframe;
            } else {
                tail->next = subframe;
                tail = subframe;
            }
        } else {
            utils_error("Error parsing subframes: Invalid subframe on line %d", json_get_lineno(stream));
        }
        /* Reads the JSON_OBJECT_END */
        json_next(stream);
    }
    /* Reads the JSON_ARRAY_END */
    json_next(stream);
}

/**
 * \brief           Parse and extract frame objects from a json stream
 *
 * Stream must point to an array of frame objects and can contain any number of
 * hem. Frame object structure is as follows:
 *  {
 *      "subframes": [
 *      ]
 *  }
 *
 * \param[in, out]  stream: Json stream pointing to an array of objects
 * \param[in, out]  asset: Asset structure to store readed frames
 */
static void
asset_parse_frames(json_stream *const restrict stream, asset_desc_t *const restrict asset) {
    asset_frame_t *tail = nullptr;

    if (json_next(stream) != JSON_ARRAY) {
        utils_error("Error parsing frames: Unknown json file format");
    }

    while (json_peek(stream) != JSON_ARRAY_END && json_get_error(stream) == nullptr) {
        asset_frame_t *frame = nullptr;

        if (json_next(stream) != JSON_OBJECT) {
            utils_error("Error parsing frames: Frame object expected on line %d", json_get_lineno(stream));
        }

        frame = malloc(sizeof(*frame));
        if (frame == nullptr) {
            utils_error("Error parsing frame: Not enought memory available");
        }
        memset(frame, 0, sizeof(*frame));

        ++asset->sheet.frames_count;
        while (json_peek(stream) != JSON_OBJECT_END && json_get_error(stream) == nullptr) {
            const char *key = nullptr;

            /* Item name string */
            if (json_next(stream) != JSON_STRING) {
                utils_error("Error parsing frames: String expected on line %d", json_get_lineno(stream));
            }
            key = json_get_string(stream, nullptr);

            /* Check frame fields */
            if (strcmp(key, "subframes") == 0) {
                asset_parse_subframes(stream, frame);
                continue;
            }
            utils_error("Error parsing frames: Unknown key '%s' on line %d", key, json_get_lineno(stream));
        }
        /* Validate readed frame */
        if (frame->subframes != nullptr) {
            if (asset->sheet.frames == nullptr) {
                asset->sheet.frames = frame;
                tail = frame;
            } else {
                tail->next = frame;
                tail = frame;
            }
        } else {
            utils_error("Error parsing frames: Invalid frame on line %d", json_get_lineno(stream));
        }
        /* Reads the JSON_OBJECT_END */
        json_next(stream);
    }
    /* Reads the JSON_ARRAY_END */
    json_next(stream);
}

/**
 * \brief           Parse and extract sheet objects from a json stream
 *
 * Stream must point to an array of sheet objects and can contain any number of
 * them. Sheet object structure is as follows:
 *  {
 *      "name": "sheet name",
 *      "file": "sheet/png/file_path",
 *      "compression": "none/zx0/slz",
 *      "type": "static/dynamic",
 *      "layout": "sprite/tilemap",
 *      "frame_w": [1, 287],
 *      "frame_h": [1, 287],
 *      "frames": [
 *      ]
 *  }
 *
 * \param[in, out]  stream: Json stream pointing to an array of objects
 * \param[in, out]  assets: Assets structure to store readed sheets
 */
static void
asset_parse_sheets(json_stream *const restrict stream, assets_t *const restrict assets) {
    asset_desc_t *tail = nullptr;

    if (json_next(stream) != JSON_ARRAY) {
        utils_error("Error parsing sheets: Array expected on line %d", json_get_lineno(stream));
    }

    while (json_peek(stream) != JSON_ARRAY_END && json_get_error(stream) == nullptr) {
        asset_desc_t *asset = nullptr;

        if (json_next(stream) != JSON_OBJECT) {
            utils_error("Error parsing sheets: Sheet object expected on line %d", json_get_lineno(stream));
        }

        asset = malloc(sizeof(*asset));
        if (asset == nullptr) {
            utils_error("Error parsing sheets: Not enought memory available");
        }
        memset(asset, 0, sizeof(*asset));

        ++assets->sheets_count;
        while (json_peek(stream) != JSON_OBJECT_END && json_get_error(stream) == nullptr) {
            char key[32] = {0};
            const char *value = nullptr;
            enum json_type items_type;

            /* Item name string */
            if (json_next(stream) != JSON_STRING) {
                utils_error("Error parsing sheets: String expected on line %d", json_get_lineno(stream));
            }
            strncpy(key, json_get_string(stream, nullptr), 32);

            /* First we check if item is an array of frames */
            if (strcmp(key, "frames") == 0) {
                asset_parse_frames(stream, asset);
                continue;
            }

            /* Remaining item's value could be strings or numbers */
            items_type = json_next(stream);
            if (items_type == JSON_STRING) {
                value = json_get_string(stream, nullptr);

                if (strcmp(key, "name") == 0) {
                    asset->name = strdup(value);
                    continue;
                }

                if (strcmp(key, "file") == 0) {
                    asset->sheet.file = strdup(value);
                    continue;
                }

                if (strcmp(key, "compression") == 0) {
                    if (strcmp(value, "none") == 0) {
                        asset->sheet.compression = ASSET_COMPRESSION_NONE;
                        continue;
                    }

                    if (strcmp(value, "zx0") == 0) {
                        asset->sheet.compression = ASSET_COMPRESSION_ZX0;
                        continue;
                    }

                    if (strcmp(value, "slz") == 0) {
                        asset->sheet.compression = ASSET_COMPRESSION_SLZ;
                        continue;
                    }
                    utils_error("Error parsing sheets: Unknown compression type '%s' on line %d", value,
                                 json_get_lineno(stream));
                }

                if (strcmp(key, "type") == 0) {
                    if (strcmp(value, "static") == 0) {
                        asset->sheet.type = ASSET_SHEET_TYPE_STATIC;
                        continue;
                    }

                    if (strcmp(value, "dynamic") == 0) {
                        asset->sheet.type = ASSET_SHEET_TYPE_DYNAMIC;
                        continue;
                    }
                    utils_error("Error parsing sheets: Unknown type '%s' on line %d", value, json_get_lineno(stream));
                }

                if (strcmp(key, "layout") == 0) {
                    if (strcmp(value, "sprite") == 0) {
                        asset->sheet.layout = ASSET_SHEET_LAYOUT_SPRITE;
                        continue;
                    }

                    if (strcmp(value, "tilemap") == 0) {
                        asset->sheet.layout = ASSET_SHEET_LAYOUT_TILEMAP;
                        continue;
                    }
                    utils_error("Error parsing sheets: Unknown layout '%s' on line %d", value,
                                 json_get_lineno(stream));
                }
                utils_error("Error parsing sheets: Unknown key '%s' on line %d", key, json_get_lineno(stream));
            }

            if (items_type == JSON_NUMBER) {
                if (strcmp(key, "frame_w") == 0) {
                    asset->sheet.frame_w = (uint16_t) json_get_number(stream);
                    continue;
                }

                if (strcmp(key, "frame_h") == 0) {
                    asset->sheet.frame_h = (uint16_t) json_get_number(stream);
                    continue;
                }
                utils_error("Error parsing sheets: Unknown key '%s' on line %d", key, json_get_lineno(stream));
            }
            utils_error("Error parsing sheets: Unknown key '%s' on line %d", key, json_get_lineno(stream));
        }
        /* Validate readed sheet */
        if (asset->name != nullptr && asset->sheet.file != nullptr && asset->sheet.frame_w > 0
            && asset->sheet.frame_w <= 287 && asset->sheet.frame_h > 0 && asset->sheet.frame_h <= 287
            && asset->sheet.frames != nullptr) {
            if (assets->sheets == nullptr) {
                assets->sheets = asset;
                tail = asset;
            } else {
                tail->next = asset;
                tail = asset;
            }
        } else {
            utils_error("Error parsing sheets: Invalid sheet on line %d", json_get_lineno(stream));
        }
        /* Reads the JSON_OBJECT_END */
        json_next(stream);
    }
    /* Reads the JSON_ARRAY_END */
    json_next(stream);
}

/**
 * \brief           Parse and extract an anim pivot object from a json stream
 *
 * Stream must point to an anim pivot object with this structure:
 *  {
 *      "x": [0, 255],
 *      "y": [0, 255]
 *  }
 *
 * \param[in, out]  stream: Json stream pointing to a pivot object
 * \param[in, out]  assets: Asset structure to store readed pivot
 */
static void
asset_parse_anim_pivot(json_stream *const restrict stream, asset_desc_t *const restrict asset) {
    if (json_next(stream) != JSON_OBJECT) {
        utils_error("Error parsing anim pivot: Subframe object expected on line %d", json_get_lineno(stream));
    }

    while (json_peek(stream) != JSON_OBJECT_END && json_get_error(stream) == nullptr) {
        char key[32] = {0};

        /* Item name string */
        if (json_next(stream) != JSON_STRING) {
            utils_error("Error parsing anim pivot: String expected on line %d", json_get_lineno(stream));
        }
        strncpy(key, json_get_string(stream, nullptr), 32);

        /* Item value stored in the json stream state */
        if (json_next(stream) != JSON_NUMBER) {
            utils_error("Error parsing anim pivot: Number expected on line %d", json_get_lineno(stream));
        }

        /* Check pivot fields */
        if (strcmp(key, "x") == 0) {
            asset->anim.pivot.x = (uint8_t) json_get_number(stream);
            continue;
        }
        if (strcmp(key, "y") == 0) {
            asset->anim.pivot.y = (uint8_t) json_get_number(stream);
            continue;
        }
        utils_error("Error parsing anim pivot: Unknown key '%s' on line %d", key, json_get_lineno(stream));
    }
    /* Reads the JSON_OBJECT_END */
    json_next(stream);
}

/**
 * \brief           Parse and extract an anim properties object from a json stream
 *
 * Stream must point to an anim properties object with this structure:
 *  {
 *      "loop": bool,
 *      "inverted": bool,
 *      "random": bool,
 *      "hide": bool
 *  }
 *
 * \param[in, out]  stream: Json stream pointing to an anim properties object
 * \param[in, out]  assets: Asset structure to store readed properties object
 */
static void
asset_parse_anim_properties(json_stream *const restrict stream, asset_desc_t *const restrict asset) {
    if (json_next(stream) != JSON_OBJECT) {
        utils_error("Error parsing anim properties: Object expected on line %d", json_get_lineno(stream));
    }

    while (json_peek(stream) != JSON_OBJECT_END && json_get_error(stream) == nullptr) {
        char key[32] = {0};
        bool val = false;
        enum json_type items_type;

        /* Item name string */
        if (json_next(stream) != JSON_STRING) {
            utils_error("Error parsing anim properties: String expected on line %d", json_get_lineno(stream));
        }
        strncpy(key, json_get_string(stream, nullptr), 32);

        /* Check if item has a boolean value */
        items_type = json_next(stream);
        if (items_type == JSON_TRUE) {
            val = true;
        } else if (items_type == JSON_FALSE) {
            val = false;
        } else {
            utils_error("Error parsing anim properties: Boolean expected on line %d", json_get_lineno(stream));
        }

        if (strcmp(key, "loop") == 0) {
            asset->anim.properties.loop = val;
            continue;
        }
        if (strcmp(key, "inverted") == 0) {
            asset->anim.properties.inverted = val;
            continue;
        }
        if (strcmp(key, "random") == 0) {
            asset->anim.properties.random = val;
            continue;
        }
        if (strcmp(key, "hide") == 0) {
            asset->anim.properties.hide = val;
            continue;
        }
        utils_error("Error parsing anim properties: Unknown key '%s' on line %d", key, json_get_lineno(stream));
    }
    /* Reads the JSON_OBJECT_END */
    json_next(stream);
}

/**
 * \brief           Parse and extract an anim sequence indexes array from a json stream
 *
 * Stream must point to a sequence indexes array with this structure:
 *  [
 *      [0..255],
 *      ...
 *      [0..255]
 *  ]
 *
 * \param[in, out]  stream: Json stream pointing to an array of indexes
 * \param[in, out]  assets: Asset structure to store readed sequence indexes
 */
static void
asset_parse_anim_sequence(json_stream *const restrict stream, asset_desc_t *const restrict asset) {
    asset_sequence_idx_t *tail = nullptr;

    if (json_next(stream) != JSON_ARRAY) {
        utils_error("Error parsing anim sequence: Unknown json file format");
    }

    while (json_peek(stream) != JSON_ARRAY_END && json_get_error(stream) == nullptr) {
        asset_sequence_idx_t *frame = nullptr;

        if (json_next(stream) != JSON_NUMBER) {
            utils_error("Error parsing anim sequence: Frame index expected on line %d", json_get_lineno(stream));
        }

        frame = malloc(sizeof(*frame));
        if (frame == nullptr) {
            utils_error("Error parsing anim sequence: Not enought memory available");
        }
        memset(frame, 0, sizeof(*frame));

        ++asset->anim.sequence_length;

        frame->idx = (uint8_t)json_get_number(stream);
        if (asset->anim.sequence == nullptr) {
            asset->anim.sequence = frame;
            tail = frame;
        } else {
            tail->next = frame;
            tail = frame;
        }
    }
    /* Reads the JSON_ARRAY_END */
    json_next(stream);
}

/**
 * \brief           Parse and extract anim objects from a json stream
 *
 * Stream must point to an array of anim objects and can contain any number of
 * them. Anim objects structure is as follows:
 *  {
 *      "name": "anim name",
 *      "sheet": "reference sheet name",
 *      "pivot": {
 *      },
 *      "properties": {
 *      },
 *      "rate": [0..255]
 *      "delay": [0..255]
 *      "sequence": [
 *      ]
 *  }
 *
 * \param[in, out]  stream: Json stream pointing to an array of objects
 * \param[in, out]  assets: Assets structure to store readed anims
 */
static void
asset_parse_anims(json_stream *const restrict stream, assets_t *const restrict assets) {
    asset_desc_t *tail = nullptr;

    if (json_next(stream) != JSON_ARRAY) {
        utils_error("Error parsing anims: Array expected on line %d", json_get_lineno(stream));
    }

    while (json_peek(stream) != JSON_ARRAY_END && json_get_error(stream) == nullptr) {
        asset_desc_t *asset = nullptr;

        if (json_next(stream) != JSON_OBJECT) {
            utils_error("Error parsing anims: Anim object expected on line %d", json_get_lineno(stream));
        }

        asset = malloc(sizeof(*asset));
        if (asset == nullptr) {
            utils_error("Error parsing anims: Not enought memory available");
        }
        memset(asset, 0, sizeof(*asset));

        ++assets->anims_count;
        while (json_peek(stream) != JSON_OBJECT_END && json_get_error(stream) == nullptr) {
            char key[32] = {0};
            const char *value = nullptr;
            enum json_type items_type;

            /* Item name string */
            if (json_next(stream) != JSON_STRING) {
                utils_error("Error parsing anims: String expected on line %d", json_get_lineno(stream));
            }
            strncpy(key, json_get_string(stream, nullptr), 32);

            if (strcmp(key, "pivot") == 0) {
                asset_parse_anim_pivot(stream, asset);
                continue;
            }

            if (strcmp(key, "properties") == 0) {
                asset_parse_anim_properties(stream, asset);
                continue;
            }

            if (strcmp(key, "sequence") == 0) {
                asset_parse_anim_sequence(stream, asset);
                continue;
            }

            /* Remaining item's value could be strings or numbers */
            items_type = json_next(stream);
            if (items_type == JSON_STRING) {
                value = json_get_string(stream, nullptr);

                if (strcmp(key, "name") == 0) {
                    asset->name = strdup(value);
                    continue;
                }

                if (strcmp(key, "sheet") == 0) {
                    asset->anim.sheet = strdup(value);
                    continue;
                }
                utils_error("Error parsing anims: Unknown key '%s' on line %d", key, json_get_lineno(stream));
            }

            if (items_type == JSON_NUMBER) {
                if (strcmp(key, "rate") == 0) {
                    asset->anim.rate = (uint8_t) json_get_number(stream);
                    continue;
                }

                if (strcmp(key, "delay") == 0) {
                    asset->anim.delay = (uint8_t) json_get_number(stream);
                    continue;
                }
                utils_error("Error parsing anims: Unknown key '%s' on line %d", key, json_get_lineno(stream));
            }
            utils_error("Error parsing anims: Unknown key '%s' on line %d", key, json_get_lineno(stream));
        }
        /* Validate readed anim */
        if (asset->name != nullptr && asset->anim.sheet != nullptr && asset->anim.rate > 0) {
            if (assets->anims == nullptr) {
                assets->anims = asset;
                tail = asset;
            } else {
                tail->next = asset;
                tail = asset;
            }
        } else {
            utils_error("Error parsing anims: Invalid anim on line %d", json_get_lineno(stream));
        }
        /* Reads the JSON_OBJECT_END */
        json_next(stream);
    }
    /* Reads the JSON_ARRAY_END */
    json_next(stream);
}

void
assets_parse(const char *const restrict path, assets_t *const restrict assets) {
    FILE *json_file = nullptr;
    json_stream stream;

    if (path == nullptr || assets == nullptr) {
        utils_error("Error parsing assets: Invalid arguments");
    }

    memset(assets, 0, sizeof(*assets));

    json_file = fopen(path, "r");
    if (json_file == nullptr) {
        utils_error("Error parsing assets: Can't open file '%s'", path);
    }

    json_open_stream(&stream, json_file);
    if (json_next(&stream) != JSON_OBJECT) {
        utils_error("Error parsing assets: Unknown json file format '%s'", path);
    }

    while (json_peek(&stream) != JSON_OBJECT_END && json_get_error(&stream) == nullptr) {
        const char *key = nullptr;

        if (json_next(&stream) != JSON_STRING) {
            utils_error("Error parsing assets: Unknown json file format '%s'", path);
        }
        key = json_get_string(&stream, nullptr);
        if (strcmp(key, "palettes") == 0) {
            asset_parse_palettes(&stream, assets);
            continue;
        }

        if (strcmp(key, "tilesets") == 0) {
            asset_parse_tilesets(&stream, assets);
            continue;
        }

        if (strcmp(key, "sheets") == 0) {
            asset_parse_sheets(&stream, assets);
            continue;
        }

        if (strcmp(key, "anims") == 0) {
            asset_parse_anims(&stream, assets);
            continue;
        }

        utils_error("Error parsing assets: Unknown json string '%s'", key);
    }

    json_close(&stream);
    fclose(json_file);
}