# MegaDrive development tools
Simple set of tools for Sega MegaDrive/Genesis hombrew development.
Some of them written by me, some not.
Everything is work in progress for my own stuff so changes expected.

## bintoc
Converts binary data files to C language data structures. It lets you specify
the desired array data type, memory alignment, size alignment, etc.

## paltool
Converts indexed png files up to 64 colors to Sega Megadrive/Genesis palette
format. Writes the resulting palette data as plain C arrays.

## tileimagetool
Extracts Sega Megadrive/Genesis plane image tiles from indexed png files up to
16 colors. Writes the resulting plane image tiles and tileset data as plain C
arrays.
Source images must be 4bpp or 8bpp png images with its size in pixels multiple
of 8.

## tilesettool
Converts indexed png files up to 16 colors to Sega Megadrive/Genesis tile
format. Writes the resulting tileset data as plain C arrays.
Source images must be 4bpp or 8bpp png images with its size in pixels multiple
of 8.

## wavtoraw
A .wav sound file format to binary format converter. This tool was previously in
the SGDK tools set.

## xgmtool
A a Sega Megadrive VGM-XGM optimization and conversion utility.

## Thanks to...
- [lodepng](https://github.com/lvandeve/lodepng) PNG encoder/decoder by Lode
  Vandevenne.
- [SGDK](https://github.com/Stephane-D/SGDK) wavtoraw tool by Stephane
  Dallongeville
- [SGDK](https://github.com/Stephane-D/SGDK) xgmtool tool by Stephane
  Dallongeville

## MIT License
Copyright (c) 2024 Juan Ángel Moreno (@_tapule)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.