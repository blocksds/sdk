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

DEFAULT="\033[0m"
RED="\033[31m"
GREEN="\033[32m"
BOLD="\033[1m"

TESTS_SUCCESS=0
TESTS_TOTAL=0

dirs=`find examples tests -iname run.py`

for dir in $dirs; do
    dir=${dir%*/test/run.py}  # Remove the trailing "/run.py"

    printf "${BOLD}[#####] TEST START: ${dir}${DEFAULT}\n"

    # Switch current folder

    pushd $dir > /dev/null

    # Build ROM

    if [ -f "build.py" ]; then
        python3 build.py --clean --build 1>test-build.log 2>&1
    elif [ -f "Makefile" ]; then
        make clean > /dev/null
        make -j`nproc` 1>test-build.log 2>&1
    elif [ -f "build.sh" ]; then
        mkdir build
        bash clean.sh 1>test-build.log 2>&1
        bash build.sh 1>test-build.log 2>&1
    else
        echo "Build system not found (build.py/Makefile/build.sh)." 1>test-build.log
        false
    fi

    rc=$?

    mv test-build.log build/

    if [ $rc -ne 0 ]; then
        printf "${RED}${BOLD}[#] BUILD FAILED: LOG START${DEFAULT}\n"
        cat build/test-build.log
        printf "${RED}${BOLD}[#] BUILD FAILED: LOG END${DEFAULT}\n"
    else
        printf "${GREEN}${BOLD}[#] BUILD SUCCEEDED${DEFAULT}\n"

        # Run test if the build has succeeded

        python3 "test/run.py" 1>build/test-run.log 2>&1

        rc=$?

        if [ $rc -ne 0 ]; then
            printf "${RED}${BOLD}[#] RUN FAILED: LOG START${DEFAULT}\n"
            cat build/test-run.log
            printf "${RED}${BOLD}[#] RUN FAILED: LOG END${DEFAULT}\n"
        else
            printf "${GREEN}${BOLD}[#] RUN SUCCEEDED${DEFAULT}\n"

            TESTS_SUCCESS=$(( TESTS_SUCCESS + 1 ))
        fi
    fi

    # Done!

    popd > /dev/null

    printf "${BOLD}[#####] TEST END: ${dir}${DEFAULT}\n"
    printf "\n"

    TESTS_TOTAL=$(( TESTS_TOTAL + 1 ))
done

TESTS_FAILURE=$(( TESTS_TOTAL - TESTS_SUCCESS ))

printf "${BOLD}[#] TESTS TOTAL:  ${TESTS_TOTAL}${DEFAULT}\n"
printf "${BOLD}[#] TESTS OK:     ${TESTS_SUCCESS}${DEFAULT}\n"
if [ ${TESTS_FAILURE} -ne 0 ]; then
    printf "${RED}${BOLD}[#] TESTS FAILED: ${TESTS_FAILURE}${DEFAULT}\n"
else
    printf "${BOLD}[#] TESTS FAILED: ${TESTS_FAILURE}${DEFAULT}\n"
fi
