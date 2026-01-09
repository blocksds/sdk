#!/bin/bash

# This script requires obj2dl from Nitro Engine:
#
# https://codeberg.org/SkyLyrac/nitro-engine
#
# You can install it with:
#
#     wf-pacman -Sy blocksds-nitroengine
#
# The model has normals, texture UV information, and no material information.

export OBJ2DL="python3 /opt/blocksds/external/nitro-engine/tools/obj2dl/obj2dl.py"

rm -rf nitrofs
mkdir nitrofs

$OBJ2DL \
    --input assets/teapot.obj \
    --output nitrofs/teapot.bin \
    --texture 256 256 \
    --scale 0.1
