#!/bin/sh

BLOCKSDS=/opt/blocksds/core
SQUEEZERW=$BLOCKSDS/tools/squeezer/squeezerw

rm graphics/atlas_texture.png
rm source/atlas.h source/atlas.c

$SQUEEZERW \
    --outputTexture graphics/atlas_texture.png \
    --outputBaseName ATLAS \
    --outputH source/atlas.h --outputC source/atlas.c \
    --verbose \
    ./original_images
