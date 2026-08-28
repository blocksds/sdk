#!/bin/bash
# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2026

# This script assumes that the architectds and blocksds_testing python packages
# are available in the current environment. For example:
#
#   sudo apt install python3-venv
#
#   python3 -m venv env
#   source env/bin/activate
#
#   pip3 install path/to/architectds.whl
#   pip3 install path/to/blocksds_testing.whl
#
# Also, you need to have a folder with the following files:
#
# - bios7.bin, bios7i.bin, bios9.bin, bios9i.bin, dsfirmware.bin, nand.bin
# - melondsds_libretro.so
#
# Then run:
#
#   export BLOCKSDS_TESTING_BLOBS=/absolute/path/to/folder/
#
# After that, you're ready to run this script **from the root of the SDK
# repository**.

set -e

dirs=`find examples tests -iname run.py`

for dir in $dirs; do
    dir=${dir%*/test/run.py}  # Remove the trailing "/run.py"

    echo "[###] TEST START: $dir"

    # Switch current folder

    pushd $dir > /dev/null

    # Build ROM

    if [ -f "build.py" ]; then
        python3 build.py --clean --build 1>test-build.log 2>&1
    else
        make clean > /dev/null
        make -j`nproc` 1>test-build.log 2>&1
    fi

    mv test-build.log build/

    # Run test

    python3 "test/run.py" 1>build/test-run.log 2>&1

    # Done!

    popd > /dev/null

    echo "[###] TEST END: $dir"
    echo ""
done
