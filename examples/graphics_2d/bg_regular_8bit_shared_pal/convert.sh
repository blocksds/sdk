#!/bin/bash

BLOCKSDS="${BLOCKSDS:-/opt/blocksds/core}"

GRIT="$BLOCKSDS/tools/grit/grit"

rm -rf graphics_source
mkdir graphics_source

# Eexport as C files
# 8 bpp, tiled mode, set magenta as transparent
# Export map, SSB layout
# Use a shared palette
# Destination folder for the converted non-shared data files
# File name (without extension) to be used for shared data
$GRIT assets/forest.png assets/forest_town.png \
    -ftc -W3 \
    -gB8 -gt -gTFFOOFF \
    -m -mLs \
    -pS \
    -Dgraphics_source \
    -Ographics_source/shared \
