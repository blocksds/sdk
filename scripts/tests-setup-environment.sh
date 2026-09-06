#!/bin/bash
# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2026

# Note: This script assumes that the architectds and melonds-ds repositories
# are in the same directory as the sdk repository.
#
#   https://codeberg.org/blocksds/architectds
#   https://github.com/JesseTG/melonds-ds

set -e

# Delete any pre-existing virtual environment
rm -rf venv

# Prepare folder to hold all required blobs
mkdir -p test_blobs
export BLOCKSDS_TESTING_BLOBS=$PWD/test_blobs

# Install venv and dependencies of melonds-ds
sudo apt install python3-venv cmake pkg-config

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
pushd ../architectds

    python -m build
    pip3 install dist/architectds-*.whl

popd

# Build the melonds-ds core
pushd ../melonds-ds

    cmake -B build
    cmake --build build # Get dependencies and build

    cp build/src/libretro/melondsds_libretro.so $BLOCKSDS_TESTING_BLOBS

popd

# Get BIOS, firmware and NAND dumps

echo "Copy the following files to $BLOCKSDS_TESTING_BLOBS"
echo ""
echo "    bios7.bin, bios7i.bin, bios9.bin, bios9i.bin, dsfirmware.bin, nand.bin"
echo ""
