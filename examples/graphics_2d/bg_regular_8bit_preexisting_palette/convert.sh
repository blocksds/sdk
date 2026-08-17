#!/bin/bash

BLOCKSDS="${BLOCKSDS:-/opt/blocksds/core}"

GRIT="$BLOCKSDS/tools/grit/grit"

rm -rf graphics_source
mkdir graphics_source

# Specify the image with the reference palette
# Export as C files
# 8 bpp, tiled mode, set magenta as transparent
# Export map, SSB layout
# Destination folder for the converted non-shared data files
# File name (without extension) to be used for shared data
$GRIT assets/rainbow_image.png \
    -fw assets/rainbow_palette.gif \
    -ftc -W3 \
    -gB8 -gt -gT000000 \
    -m -mLs -mRt \
    -Dgraphics_source \
    -Ographics_source/shared \
