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

#ifndef IMG4BPP_H
#define IMG4BPP_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief           Create a 4bpp image from a 8bpp image data buffer
 * \param[in]       image: 8bpp image data buffer
 * \param[in]       size: Size of image data in bytes (equals pixels)
 * \return          The new 4bpp image data buffer
 */
uint8_t *img4bpp_from_8bpp(uint8_t *const image, const uint32_t size);

/**
 * \brief           Extracts 8x8 pixel tiles from a 4bpp image
 * \param[in]       image: Source 4bpp image to extract the tiles from
 * \param[in]       width: Width in pixels of source 4bpp image
 * \param[in]       height: Height in pixels of source 4bpp image
 * \return          Pointer to a buffer containing the extracted tiles
 */
uint8_t *img4bpp_extract_tiles(uint8_t *const image, const uint32_t width, const uint32_t height);

#ifdef __cplusplus
}
#endif

#endif /* IMG4BPP_H */