# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: ds-sloth, 2024

# Written to mimic devkitPro defaults to reduce migration surprises.

foreach(LANG IN ITEMS C CXX ASM)
    set(CMAKE_${LANG}_OUTPUT_EXTENSION .o)
    set(CMAKE_${LANG}_OUTPUT_EXTENSION_REPLACE 1)

    set(CMAKE_${LANG}_FLAGS_DEBUG_INIT          " -g -Og -DDEBUG")
    set(CMAKE_${LANG}_FLAGS_MINSIZEREL_INIT     " -g -Oz -DNDEBUG")
    set(CMAKE_${LANG}_FLAGS_RELEASE_INIT        " -g -O2 -DNDEBUG")
    set(CMAKE_${LANG}_FLAGS_RELWITHDEBINFO_INIT " -g -O2 -DNDEBUG")
endforeach()
