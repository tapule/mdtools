/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of mdtools (MegaDrive development tools).
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2025
 * Github: https://github.com/tapule/mdtools
 */

/**
 * \file            tilesets.h
 * \brief           Tilesets conversion utility functions
 */

#ifndef TILESETS_H
#define TILESETS_H

#include <stdint.h>
#include "assets_parser.h"
#include "cli_parser.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief           Convert tilesets assets to Sega Megadrive/Genesis format and
 *                  export them to C header and source files.
 * \param[in]       config: Configuration including output path for C files
 * \param[in]       assets: Structured data incluing tilesets asset descriptions
 * \note            The execution will be terminated if something went wrong
 */
void tilesets_process(args_t *const restrict config, assets_t *const restrict assets);

#ifdef __cplusplus
}
#endif

#endif /* TILESETS_H */