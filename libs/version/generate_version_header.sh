#!/bin/bash

# SPDX-License-Identifier: Zlib
#
# Copyright (C) 2025 Antonio Niño Díaz

if [[ $# -ne 4 ]]; then
    echo "Invalid number of arguments"
    echo
    echo "Usage: $0 <version> <out header path> <out makefile path> <fixed_makefile_part path>"
    exit 1
fi

VERSION_STRING=$1
OUT_HEADER_PATH=$2
OUT_MAKEFILE_PATH=$3
FIXED_MAKEFILE_PATH=$4

# Split version string into its components
# ========================================

if [[ "${VERSION_STRING}" == "DEV" ]]; then
    major="0"
    minor="0"
    patch="0"
    #version_extra=""
    tagged_version=0
else
    # Check if the version string has extra data after the version number
    if [[ "${VERSION_STRING}" == *"-"* ]]; then
        version_base=`echo ${VERSION_STRING} | cut -d- -f1`
        #version_extra=`echo ${VERSION_STRING} | cut -d- -f2`
        tagged_version=0
    else
        version_base="${VERSION_STRING}"
        #version_extra=""
        tagged_version=1
    fi

    if [[ "$version_base" =~ ^v(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)$ ]]; then
        major="${BASH_REMATCH[1]}"
        minor="${BASH_REMATCH[2]}"
        patch="${BASH_REMATCH[3]}"
    else
        echo "regex didn't match '$version_base'"
        exit 1
    fi
fi

# Export C header
# ===============

cat <<EOF > ${OUT_HEADER_PATH}
// SPDX-License-Identifier: Zlib
//
// Copyright (C) 2025 Antonio Niño Díaz

#ifndef BLOCKSDS_VERSION_H__
#define BLOCKSDS_VERSION_H__

/// Major part of the version
#define BLOCKSDS_VERSION_MAJOR ${major}
/// Minor part of the version
#define BLOCKSDS_VERSION_MINOR ${minor}
/// Patch part of the version
#define BLOCKSDS_VERSION_PATCH ${patch}

// Macro to compose an integer that represents a version number
#define BLOCKSDS_VERSION_MAKE(major, minor, patch) \\
    ((major << 16) | (minor << 8) | (patch << 0))

/// Integer that represents the version number of this build
#define BLOCKSDS_VERSION_FULL \\
    BLOCKSDS_VERSION_MAKE(BLOCKSDS_VERSION_MAJOR, BLOCKSDS_VERSION_MINOR, BLOCKSDS_VERSION_PATCH)

/// Full version string
#define BLOCKSDS_VERSION_STRING "${VERSION_STRING}"

/// This is 1 if the current build of BlocksDS is a tagged (official) release
#define BLOCKSDS_VERSION_IS_TAGGED ${tagged_version}

// Returns true if the version of BlocksDS is the specified one
#define BLOCKSDS_VERSION_EQUALS(major, minor, patch) \\
    (BLOCKSDS_VERSION_FULL == BLOCKSDS_VERSION_MAKE(major, minor, patch))

// Returns true if the version of BlocksDS is at least the specified one
#define BLOCKSDS_VERSION_AT_LEAST(major, minor, patch) \\
    (BLOCKSDS_VERSION_FULL >= BLOCKSDS_VERSION_MAKE(major, minor, patch))

// Returns true if the version of BlocksDS is at most the specified one
#define BLOCKSDS_VERSION_AT_MOST(major, minor, patch) \\
    (BLOCKSDS_VERSION_FULL <= BLOCKSDS_VERSION_MAKE(major, minor, patch))

#endif // BLOCKSDS_VERSION_H___
EOF

# Export Makefile
# ===============

cat <<EOF > ${OUT_MAKEFILE_PATH}
# SPDX-License-Identifier: Zlib
#
# Copyright (C) 2025 Antonio Niño Díaz

# Individual version components
BLOCKSDS_VERSION_MAJOR = ${major}
BLOCKSDS_VERSION_MINOR = ${minor}
BLOCKSDS_VERSION_PATCH = ${patch}

# Full version string
BLOCKSDS_VERSION_STRING = ${VERSION_STRING}

# This is 1 if the current build of BlocksDS is a tagged (official) release
BLOCKSDS_VERSION_IS_TAGGED = ${tagged_version}

EOF

cat ${FIXED_MAKEFILE_PATH} >> ${OUT_MAKEFILE_PATH}

exit 0
