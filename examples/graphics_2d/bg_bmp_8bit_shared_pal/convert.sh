#!/bin/bash

BLOCKSDS="${BLOCKSDS:-/opt/blocksds/core}"

GRIT="$BLOCKSDS/tools/grit/grit"

rm -rf graphics_source
mkdir graphics_source

# Eexport as C files
# 8 bpp, bitmap mode, set magenta as transparent
# Use a shared palette
# Destination folder for the converted non-shared data files
# File name (without extension) to be used for shared data
$GRIT assets/manga_bg_01.png assets/manga_bg_07.png \
    -ftc -W3 \
    -gB8 -gb -gTFFOOFF \
    -pS \
    -Dgraphics_source \
    -Ographics_source/shared \
