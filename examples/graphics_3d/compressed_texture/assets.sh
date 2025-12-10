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
