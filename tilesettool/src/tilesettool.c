/* SPDX-License-Identifier: MIT */
/**
 * -- MegaDrive development tools --
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2024
 * Github: https://github.com/tapule/mdtools
 *
 * tilesettool v0.02
 *
 * A Sega Megadrive/Genesis image tileset extractor
 *
 * Extracts Sega Megadrive/Genesis tiles from 8bpp indexed png files up to 16
 * colors.
 *
 * Usage example: tilesettool -s pngs/path -d dest/path  -n res_til
 *
 * It extracts tilesets in "pngs/path/*.png" and generates the C source files
 * "res_til.h" and "res_til.c" in "dest/path" directory.
 * For each png file, tilesettool adds a define with its size in tiles and a
 * const uint32_t array containing the tileset data (one tile a row).
 *
 * If -s parameter is not specified, the current directory will be used as
 * source folder.
 * If -d parameter is not specified, the current directory will be used as
 * destination folder.
 *
 * If "mytileset.png" is in "pngs/path" the previos example usage generates:
 *
 * dest/path/res_til.h
 * #ifndef RES_TIL_H
 * #define RES_TIL_H
 *
 * #include <stdint.h>
 *
 * #define RES_TIL_MYTILESET_SIZE    3
 *
 * extern const uint32_t res_til_mytileset[RES_TIL_MYTILESET_SIZE * 8];
 *
 * #endif // RES_TIL_H
 *
 * dest/path/res_til.c
 * #include "res_til.h"
 *
 * const uint32_t res_til_mytileset[RES_TIL_MYTILESET_SIZE * 8] = {
 *    0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111,
 *    0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111211,
 *    0x11111221, 0x11111221, 0x11111222, 0x21111222, 0x21112221, 0x21112211, 0x21111111, 0x21111111
 * };
 *
 * You can extract tiles from a unique file too:
 *  tilesettool -s pngs/path/file.png -d dest/path -n res_pal
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include "lodepng.h"

#define MAX_TILESETS            512	    /* Enough?? */
#define MAX_FILE_NAME_LENGTH    128     /* Max length for file names */
#define MAX_PATH_LENGTH         1024    /* Max length for paths */

#define PARAMS_ERROR            0   /* Error en procesado de parámetros */
#define PARAMS_STOP             1   /* Procesado de parámetros ok, finalizar */
#define PARAMS_CONTINUE         2   /* Procesado de parámetros ok, procesar */

const char version_text [] =
    "tilesettool v0.02\n"
    "A Sega Megadrive/Genesis image tileset extractor\n"
    "Coded by: Juan Ángel Moreno Fernández (@_tapule) 2024\n"
    "Github: https://github.com/tapule/mdtools\n";

const char help_text [] =
    "usage: tilesettool [options]\n"
    "\n"
    "Options:\n"
    "  -v, --version       Show version information and exit\n"
    "  -h, --help          Show this help message and exit\n"
    "  -s <path>|<file>    Use a directory path to look for png files\n"
    "                      or a unique png file to extract tiles from"
    "                      Current directory will be used as default\n"
    "  -d <path>           Use a path to save generated C source files\n"
    "                      The current directory will be used as default\n"
    "  -n <name>           Use name as prefix for files, defines, vars, etc\n"
    "                      If it is not specified, \"til\" will be used as\n"
    "                      default for multiple files. Source file name itself\n"
    "                      will be used if there is only one source file\n";

/* Stores the input parameters */
typedef struct params_t
{
    char *src_path;   /* Folder with the source images in png files */
    char *dest_path;  /* Destination folder for the generated .h and .c */
    char *dest_name;  /* Base name for the generated .h and .c files */
} params_t;

/* Stores tileset's data */
typedef struct tileset_t
{
    char file[MAX_FILE_NAME_LENGTH];        /* Original png file */
    char name[MAX_FILE_NAME_LENGTH];        /* Name without the extension */
    char size_define[MAX_FILE_NAME_LENGTH]; /* Size constant define name  */
    uint8_t *data;                          /* Tiles storage */
    uint16_t size;                          /* Tileset size in tiles */
} tileset_t;

/* Global storage for the parsed tilesets */
tileset_t tilesets[MAX_TILESETS];

/**
 * @brief Convert a string to upper case
 *
 * @param str string to convert
 */
void strtoupper(char *str)
{
    char *c;
    c = str;

    while (*c)
    {
        *c = toupper(*c);
        ++c;
    }
}

/**
 * @brief Converts a 8bpp image data buffer to 4bpp
 *
 * @param image 8bpp image data buffer
 * @param size Size of image data in bytes (equals pixels)
 * @return uint8_t* The new 4bpp image data buffer
 */
uint8_t *image_to_4bpp(uint8_t *image, const uint32_t size)
{
    uint8_t *image_4bpp = NULL;
    uint8_t *src = NULL;
    uint8_t *dest = NULL;
    uint32_t i;

    /* Requests memory for the new image which needs half the original */
    image_4bpp = malloc(size / 2);
    if (!image_4bpp)
    {
        return NULL;
    }
    src = image;
    dest = image_4bpp;

    /* Converts two bytes in 8bpp (2 pixels) to one byte in 4bpp */
    for (i = 0; i < size / 2; ++i)
    {
        *dest = ((src[0] & 0x0F) << 4) | ((src[1] & 0x0F) << 0);
        ++dest;
        src += 2;
    }

    return image_4bpp;
}

/**
 * @brief Extracts 8x8 pixel tiles from a 4bpp image
 *
 * @param image Source image to extract the tiles from
 * @param width Width in pixels of source image
 * @param height Height in pixels of source image
 * @return uint8_t* Buffer containing the extracted tiles
 */
uint8_t *image_4bpp_to_tile(uint8_t *image, const uint32_t width, const uint32_t height)
{
    uint32_t tile_width;    /* Width in tiles of our image */
    uint32_t tile_height;   /* Height in tiles of our image */
    uint8_t *tiles;         /* Memory storage for our tiles */
    uint8_t *tiles_p;       /* Current position in the tiles memory */
    uint8_t *image_p;       /* Current position in the image memory */
    uint32_t pitch;         /* Jump to the next tile row */
    uint32_t tile_x;        /* Tile x positon counter */
    uint32_t tile_y;        /* Tile y position counter */
    uint32_t tile_row;      /* Row copy position counter */

    /* Image dimesions are in pixels, convert to tiles */
    tile_width = width / 8;
    tile_height = height / 8;

    /*
     A tile is 32 bytes, 8 rows of 4 bytes each. Pitch is the jump in bytes in
     the original image to point to the start of the next row in a tile
    */
    pitch =  tile_width * 4;

    /* Requests 32 bytes of memory for each tile */
    tiles = malloc(tile_width * tile_height * 32);
    if (!tiles)
    {
        return NULL;
    }

    /* Point to the start of tiles buffer */
    tiles_p = tiles;

    for (tile_y = 0; tile_y < tile_height; ++tile_y)
    {
        for (tile_x = 0; tile_x < tile_width; ++tile_x)
        {
            /* Move the image pointer to the start of next tile to process */
            image_p = &image[((tile_y * 8) * pitch) + (tile_x * 4)];

            /* Put current tile's rows in the tiles buffer */
            for(tile_row = 0; tile_row < 8; ++tile_row)
            {
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

/**
 * @brief Parses the input parameters
 *
 * @param argc Input arguments counter
 * @param argv Input arguments vector
 * @param params Where to store the input paramss
 * @return 0 if there was an error
 *         1 if the arguments parse was ok but we must end (-v or -h)
 *         2 if the arguments parse was ok and we can continue
 */
uint8_t parse_params(uint32_t argc, char** argv, params_t *params)
{
    uint32_t i;

    i = 1;
    while (i < argc)
    {
        if ((strcmp(argv[i], "-v") == 0) || (strcmp(argv[i], "--version") == 0))
        {
            fputs(version_text, stdout);
            return PARAMS_STOP;
        }
        else if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0))
        {
            fputs(help_text, stdout);
            return PARAMS_STOP;
        }
        /* Source path where png files are */
        else if (strcmp(argv[i], "-s") == 0)
        {
            if (i < argc - 1)
            {
                params->src_path = argv[i + 1];
                ++i;
            }
            else
            {
                fprintf(stderr, "%s: an argument is needed for this option: '%s'\n",
                        argv[0], argv[i]);
                return PARAMS_ERROR;
            }
        }
        /* Destination path to save the generated .h and .c files */
        else if (strcmp(argv[i], "-d") == 0)
        {
            if (i < argc - 1)
            {
                params->dest_path = argv[i + 1];
                ++i;
            }
            else
            {
                fprintf(stderr, "%s: an argument is needed for this option: '%s'\n",
                        argv[0], argv[i]);
                return PARAMS_ERROR;
            }
        }
        /* Base name for variables, defines, and  generated .h and .c files */
        else if (strcmp(argv[i], "-n") == 0)
        {
            if (i < argc - 1)
            {
                params->dest_name = argv[i + 1];
                ++i;
            }
            else
            {
                fprintf(stderr, "%s: an argument is needed for this option: '%s'\n",
                        argv[0], argv[i]);
                return PARAMS_ERROR;
            }
        }
        else
        {
            fprintf(stderr, "%s: unknown option: '%s'\n", argv[0], argv[i]);
            return PARAMS_ERROR;
        }
        ++i;
    }
    return PARAMS_CONTINUE;
}

/**
 * @brief Processes a png image file and extracts its tiles in Megadrive format
 *
 * @param path File path
 * @param file Png image file to process
 * @param tileset_index Index in the tilesets array to store the data
 * @return 0 if success, lodepng error code in other case
 */
uint32_t tileset_read(const char* path, const char *file,
                      const uint32_t tileset_index)
{
    char file_path[MAX_PATH_LENGTH];
    char *file_ext;
    uint32_t error;
    uint8_t *png_data = NULL;
    size_t png_size;
    LodePNGState png_state;
    uint8_t *image_data = NULL;
    uint32_t image_width;
    uint32_t image_height;
    uint8_t *image_4bpp = NULL;
    uint32_t i;

    /* Builds the complete file path */
    strcpy(file_path, path);
    strcat(file_path, "/");
    strcat(file_path, file);
    printf("File %s\n", file_path);

    /* Load the file into a memory buffer, no png checks here */
    error = lodepng_load_file(&png_data, &png_size, file_path);
    if (error)
    {
        free(png_data);
        printf(lodepng_error_text(error));
        return error;
    }

    lodepng_state_init(&png_state);
    /* Get colors and pixels without conversion */
    png_state.decoder.color_convert = false;
    /* Decode our png image */
    error = lodepng_decode(&image_data, &image_width, &image_height, &png_state,
                           png_data, png_size);
    free(png_data);
    /* Checks for errors in the decode stage */
    if (error)
    {
        printf("\tSkiping file: ");
        printf(lodepng_error_text(error));
        putchar('\n');
        return error;
    }

    /* Checks if the image is an indexed one */
    if (png_state.info_png.color.colortype != LCT_PALETTE)
    {
        printf("\tSkiping file: The image must be in indexed color mode\n");
        return 1;
    }

    /* Checks if the image is a 4bpp or 8bpp one */
    if (png_state.info_png.color.bitdepth != 4 &&
        png_state.info_png.color.bitdepth != 8)
    {
        printf("\tSkiping file: %d bpp not suported. Only 4bpp and 8bpp png files supported \n");
        return 1;
    }

    /* Checks if the image has more than 16 colors */
    if (png_state.info_png.color.palettesize > 16)
    {
        printf("\tSkiping file: More than 16 colors png image detected. \n");
        return 1;
    }

    /* Checks if image width is multiple of 8 */
    if (image_width % 8)
    {
        printf("\tSkiping file: Image width is not multiple of 8. \n");
        return 1;
    }

    /* Checks if image height is multiple of 8 */
    if (image_height % 8)
    {
        printf("\tSkiping file: Image height is not multiple of 8. \n");
        return 1;
    }

    /* Converts the image to Megadrive 4bpp format only if it is 8bpp */
    if (png_state.info_png.color.bitdepth == 8)
    {
        image_4bpp = image_to_4bpp(image_data, image_width * image_width);
        free(image_data);

        if (!image_4bpp)
        {
            printf("\tError: Can't conver image to 4bpp. \n");
            return 1;
        }
    }
    else
    {
        image_4bpp = image_data;
    }

    /* Extract the tileset from our 4bpp image data */
    tilesets[tileset_index].data = image_4bpp_to_tile(image_4bpp,
                                                      image_width,
                                                      image_height);
    free(image_4bpp);

    /* Save the tileset file name and tile size */
    strcpy(tilesets[tileset_index].file, file);
    tilesets[tileset_index].size = (image_width / 8) * (image_height / 8);

    /* Save the tileset name without the extension in the tileset struct */
    strcpy( tilesets[tileset_index].name, file);
    file_ext = strrchr( tilesets[tileset_index].name, '.');
    if (file_ext)
    {
        *file_ext = '\0';
    }

    return 0;
}

/**
 * @brief Builds the C header file for the generated tilesets
 *
 * @param path Destinatio path for the .h file
 * @param name Base name for the .h file (name + .h)
 * @param use_prefix Indicate if a prefix should be used for vars, etc.
 * @param tileset_count Number of tilesets to process from the global tilesets
 * @return true if everythig was correct, false otherwise
 */
bool build_header_file(const char *path, const char *name,
                       const bool use_prefix, const uint32_t tileset_count)
{
    FILE *h_file;
    char buff[1024];
    uint32_t i, j;

    /* Builds the .h complete file path */
    strcpy(buff, path);
    strcat(buff, "/");
    strcat(buff, name);
    strcat(buff, ".h");

    h_file = fopen(buff, "w");
    if (!h_file)
    {
        return false;
    }

    /* An information message */
    fprintf(h_file, "/* Generated with tilesettool v0.02                    */\n");
    fprintf(h_file, "/* a Sega Megadrive/Genesis image tileset extractor    */\n");
    fprintf(h_file, "/* Github: https://github.com/tapule/mdtools             */\n\n");

    /* Header include guard */
    strcpy(buff, name);
    strtoupper(buff);
    strcat(buff, "_H");
    fprintf(h_file, "#ifndef %s\n", buff);
    fprintf(h_file, "#define %s\n\n", buff);
    fprintf(h_file, "#include <stdint.h>\n\n");

    /* Tileset sizes defines */
    for (i = 0; i < tileset_count; ++i)
    {
        tilesets[i].size_define[0] = '\0';
        if (use_prefix)
        {
            strcpy(tilesets[i].size_define, name);
            strcat(tilesets[i].size_define, "_");
        }
        strcat(tilesets[i].size_define, tilesets[i].name);
        strcat(tilesets[i].size_define, "_SIZE");
        strtoupper(tilesets[i].size_define);
        fprintf(h_file, "#define %s    %d\n", tilesets[i].size_define,
                tilesets[i].size);
    }
    fprintf(h_file, "\n");

    /* Tilesets declarations */
    for (i = 0; i < tileset_count; ++i)
    {
        buff[0] = '\0';
        if (use_prefix)
        {
            strcpy(buff, name);
            strcat(buff, "_");
        }
        strcat(buff, tilesets[i].name);
        fprintf(h_file, "extern const uint32_t %s[%s * 8];\n", buff,
                tilesets[i].size_define);
    }
    fprintf(h_file, "\n");

    /* End of header include guard */
    strcpy(buff, name);
    strtoupper(buff);
    strcat(buff, "_H");
    fprintf(h_file, "#endif /* %s */\n", buff);

    fclose(h_file);
    return true;
}

/**
 * @brief Builds the C source file for the extracted tilesets
 *
 * @param path Destinatio path for the .c file
 * @param name Base name for the .c file (name + .c)
 * @param use_prefix Indicate if a prefix should be used for files, vars, etc.
 * @param tileset_count Number of tilesets to process from the global storage
 * @return true if everythig was correct, false otherwise
 */
bool build_source_file(const char *path, const char *name,
                       const bool use_prefix, const uint32_t tileset_count)
{
    FILE *c_file;
    char buff[1024];
    uint32_t tileset;   /* Current tileset to process */
    uint32_t tile;      /* Current tile to process */
    uint32_t row;       /* Current row of pixels in a tile */

    /* Builds the .c complete file path */
    strcpy(buff, path);
    strcat(buff, "/");
    strcat(buff, name);
    strcat(buff, ".c");

    c_file = fopen(buff, "w");
    if (!c_file)
    {
        return false;
    }

    /* Header include */
    strcpy(buff, name);
    strcat(buff, ".h");
    fprintf(c_file, "#include \"%s\"\n\n", buff);

    for (tileset = 0; tileset < tileset_count; ++tileset)
    {
        buff[0] = '\0';
        if (use_prefix)
        {
            strcpy(buff, name);
            strcat(buff, "_");
        }
        strcat(buff, tilesets[tileset].name);
        fprintf(c_file, "const uint32_t %s[%s * 8] = {", buff,
                tilesets[tileset].size_define);
        for (tile = 0; tile < tilesets[tileset].size; ++tile)
        {
            /* Do we need to write a comma after the las value? */
            if (tile)
            {
                fprintf(c_file, ", ");
            }
            /* Separate tile definition from text line start */
            fprintf(c_file, "\n    ");
            /* Writes all the tile's rows in a single line */
            for (row = 0; row < 8; ++row)
            {
                /* Each tile row is 4 hex values */
                fprintf(c_file, "0x");
                fprintf(c_file, "%02X", tilesets[tileset].data[(tile * 32) + (row * 4)]);
                fprintf(c_file, "%02X", tilesets[tileset].data[(tile * 32) + (row * 4) + 1]);
                fprintf(c_file, "%02X", tilesets[tileset].data[(tile * 32) + (row * 4) + 2]);
                fprintf(c_file, "%02X", tilesets[tileset].data[(tile * 32) + (row * 4) + 3]);
                /* Add a separator for each tile row definition */
                if (row < 7)
                {
                    fprintf(c_file, ", ");
                }
            }
         }

        fprintf(c_file, "\n};\n\n");
    }

    fclose(c_file);
    return true;
}

int main(int argc, char **argv)
{
    params_t params = {0};
    uint32_t tileset_index = 0;
    DIR *dir;
    char *file_name;
    struct dirent *dir_entry;
    uint8_t params_status;

    /* Set default values here */
    params.src_path = ".";
    params.dest_path = ".";

    /* Argument reading and processing */
    params_status = parse_params(argc, argv, &params);
    if (params_status == PARAMS_ERROR)
    {
        return EXIT_FAILURE;
    }
    if (params_status == PARAMS_STOP)
    {
        return EXIT_SUCCESS;
    }

    /* First try to open source path as a directory */
    dir = opendir(params.src_path);
    if (dir != NULL)
    {
        printf(version_text);
        printf("\nReading files...\n");
        while ((dir_entry = readdir(dir)) != NULL)
        {
            /* Checks max allowed tilesets */
            if (tileset_index >= MAX_TILESETS)
            {
                closedir(dir);
                fprintf(stderr, "Error: More than %d files in the source directory\n", MAX_TILESETS);
                return EXIT_FAILURE;
            }

            /* Process only regular files */
            if (dir_entry->d_type == DT_REG)
            {
                if (!tileset_read(params.src_path, dir_entry->d_name, tileset_index))
                {
                    printf("\tPng file to tiles: %s -> %s\n", dir_entry->d_name,
                        tilesets[tileset_index].name);
                    ++tileset_index;
                }
            }
        }
        closedir(dir);
    }
    /* We can't open source path as directory, try to open it as file instead */
    else
    {
        /* Get the file name and path */
        file_name = strrchr(params.src_path, '/');
        if (file_name)
        {
            *file_name = '\0';
            ++file_name;
        }
        else
        {
            file_name = params.src_path;
            params.src_path = ".";
        }
        printf(version_text);
        printf("\nReading file...\n");
        if (!tileset_read(params.src_path, file_name, tileset_index))
        {
            printf("\tFile to binary: %s -> %s\n", file_name,
                tilesets[tileset_index].name);
            ++tileset_index;
        }
    }

    printf("%d tilesets readed.\n", tileset_index);

    if (tileset_index > 0)
    {
        /* By default use BASE_NAME as prefix for files, defines, vars, etc */
        bool use_prefix = true;

        /* Adjust the destination base name if it was not specified */
        if (!params.dest_name)
        {
            /* Only one file, use its name as base name and no prefix */
            if (tileset_index == 1)
            {
                params.dest_name = tilesets[0].name;
                use_prefix = false;
            }
            /* More than one file, use "bins" as base name */
            else
            {
                params.dest_name = "til";
            }
        }

        printf("Building C header file...\n");
        build_header_file(params.dest_path, params.dest_name, use_prefix,
                          tileset_index);
        printf("Building C source file...\n");
        build_source_file(params.dest_path, params.dest_name, use_prefix,
                          tileset_index);
        printf("Done.\n");
    }

    return EXIT_SUCCESS;
}
