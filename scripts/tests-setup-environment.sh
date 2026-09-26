#!/bin/bash
# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2026

# 1. Before running the script, install venv and the dependencies of melonDS DS:
#
#      sudo apt install python3-venv cmake pkg-config
#
# 2. This script assumes that the architectds and melonds-ds repositories are in
#    the same directory as the sdk repository.
#
#      https://codeberg.org/blocksds/architectds
#      https://github.com/JesseTG/melonds-ds
#
#    If they aren't found, they will be cloned instead.

set -e

# Install dependencies
export PATH=$PATH:/opt/wonderful/bin
wf-pacman -Syu \
    blocksds-libcurl blocksds-mbedtls blocksds-ncurses \
    blocksds-ptexconv \
    toolchain-llvm-teak-llvm \
    wf-nnpack wf-superfamiconv \

# Delete any pre-existing virtual environment
rm -rf env

# Prepare folder to hold all required blobs
mkdir -p test_blobs
export BLOCKSDS_TESTING_BLOBS=$PWD/test_blobs

# Create and activate a virtual environment
python3 -m venv env
source env/bin/activate

# Install tools required to create Python wheels
pip3 install setuptools build

# Create and install blocksds_testing wheel
pushd scripts/blocksds_testing/

    python -m build
    pip3 install dist/blocksds_testing-*.whl

popd

# Create and install ArchitectDS wheel

if [ ! -d "../architectds" ]; then

    pushd ..
    git clone https://codeberg.org/blocksds/architectds
    popd

fi

pushd ../architectds

    python -m build
    pip3 install dist/architectds-*.whl

popd

# Build the melonDS DS core

if [ ! -d "../melonds-ds" ]; then

    pushd ..
    git clone https://github.com/JesseTG/melonds-ds
    popd

fi

pushd ../melonds-ds

    # This is the commit that the tests currently use
    git checkout 86c37f3ea4d00d8887016bb0aa9e722bb9edba9b

    cmake -B build
    cmake --build build # Get dependencies and build

    cp build/src/libretro/melondsds_libretro.so $BLOCKSDS_TESTING_BLOBS

popd

# Get BIOS, firmware and NAND dumps

echo "Copy the following files to $BLOCKSDS_TESTING_BLOBS"
echo ""
echo "    bios7.bin, bios7i.bin, bios9.bin, bios9i.bin, dsfirmware.bin, nand.bin"
echo ""
echo "IMPORTANT: You need to setup the default melonDS Access Point in the firmware."
