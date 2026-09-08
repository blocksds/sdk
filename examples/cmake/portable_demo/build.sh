#!/bin/bash

set -e

export WONDERFUL_TOOLCHAIN="${WONDERFUL_TOOLCHAIN:-/opt/wonderful}"
export BLOCKSDS="${BLOCKSDS:-/opt/blocksds/core}"

bash clean.sh

cmake -B build -DCMAKE_TOOLCHAIN_FILE=$BLOCKSDS/cmake/BlocksDS.cmake
cmake --build build -j$(nproc --all)
cp build/*.nds .
