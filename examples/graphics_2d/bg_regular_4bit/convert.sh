#!/bin/bash

rm -rf data
mkdir -p data

SUPERFAMICONV=/opt/wonderful/bin/wf-superfamiconv

${SUPERFAMICONV} --verbose \
    --mode gba \
    --color-zero FF00FF \
    --in-image assets/forest.png \
    --out-palette data/palette.bin \
    --out-tiles data/tiles.bin \
    --out-map data/map.bin
