#!/bin/bash

# TODO: Replace this by a real build system

set -e

BLOCKSDS="${BLOCKSDS:-/opt/blocksds/core}"

pushd lib_calculator
make clean
make
popd

rm -rf nitrofs
mkdir nitrofs

make build/dyn_libs_basic_demo.elf

${BLOCKSDS}/tools/dsltool/dsltool \
    -i lib_calculator/calculator.elf \
    -o nitrofs/calculator.dsl \
    -m build/dyn_libs_basic_demo.elf

make clean
make
