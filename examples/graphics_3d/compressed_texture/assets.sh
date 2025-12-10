#!/bin/sh

rm -rf data
mkdir -p data

# ptexconv can be obtained here: https://github.com/Garhoogin/ptexconv/
#
# It can also be installed with wf-pacman:
#
#     wf-pacman -Sy blocksds-ptexconv

/opt/blocksds/external/ptexconv/ptexconv \
    -gt -ob -k FF00FF -v -f tex4x4 \
    -o data/neon \
    assets/neon.png

# Concatenate texel blocks and palette indices into a single file.
cat data/neon_tex.bin data/neon_idx.bin > data/neon_combined.bin
rm data/neon_tex.bin data/neon_idx.bin
