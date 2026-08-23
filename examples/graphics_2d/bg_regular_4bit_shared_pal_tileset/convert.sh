#!/bin/bash

IN=assets
OUT=data

rm -rf ${OUT}
mkdir -p ${OUT}

SUPERFAMICONV=/opt/wonderful/bin/wf-superfamiconv

${SUPERFAMICONV} palette \
    --mode gba \
    --palettes 16 \
    --colors 16 \
    --color-zero FF00FF \
    --in-image ${IN}/tiny_16.png \
    --out-data ${OUT}/palette.bin \
    --verbose

${SUPERFAMICONV} tiles \
    --mode gba \
    --bpp 4 \
    --tile-width 8 --tile-height 8 \
    --max-tiles 1024 \
    --in-image ${IN}/tiny_16.png \
    --in-palette ${OUT}/palette.bin \
    --out-data ${OUT}/tileset.bin \
    --no-flip --no-discard \
    --verbose

# Maps

${SUPERFAMICONV} map \
    --mode gba \
    --bpp 4 \
    --tile-width 8 --tile-height 8 \
    --tile-base-offset 0 \
    --palette-base-offset 0 \
    --map-width 32 --map-height 32 \
    --split-width 32 --split-height 32 \
    --in-image ${IN}/map_1.png \
    --in-palette ${OUT}/palette.bin \
    --in-tiles ${OUT}/tileset.bin \
    --out-data ${OUT}/map_1.bin \
    --no-flip \
    --verbose

${SUPERFAMICONV} map \
    --mode gba \
    --bpp 4 \
    --tile-width 8 --tile-height 8 \
    --tile-base-offset 0 \
    --palette-base-offset 0 \
    --map-width 32 --map-height 32 \
    --split-width 32 --split-height 32 \
    --in-image ${IN}/map_2.png \
    --in-palette ${OUT}/palette.bin \
    --in-tiles ${OUT}/tileset.bin \
    --out-data ${OUT}/map_2.bin \
    --no-flip \
    --verbose
