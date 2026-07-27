#!/bin/bash

BLOCKSDS="${BLOCKSDS:-/opt/blocksds/core}"

GRIT="$BLOCKSDS/tools/grit/grit"

rm -rf graphics_source
mkdir graphics_source

# Eexport as C files
# 4 bpp, tiled mode, set magenta as transparent
# Export map, SSB layout, reduce with options optimized for 4 bpp map
# Use a shared palette and shared graphics (tile set)
# Destination folder for the converted non-shared data files
# File name (without extension) to be used for shared data
$GRIT assets/forest.png assets/forest_town.png \
    -ftc -W3 \
    -gB4 -gt -gTFFOOFF \
    -m -mLs -mR4 \
    -pS -gS \
    -Dgraphics_source \
    -Ographics_source/shared \
