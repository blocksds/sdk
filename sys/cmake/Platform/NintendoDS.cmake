# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: ds-sloth, 2024

cmake_minimum_required(VERSION 3.16)

# set default build configs to standard values
set(CMAKE_BUILD_TYPE_INIT "Release")
set(CMAKE_EXECUTABLE_SUFFIX ".elf")
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "Dynamic libraries are unsupported on BlocksDS")
set_property(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS FALSE)



# set up compiler and linker options
set(STANDARD_INCLUDE_DIRECTORIES "${BLOCKSDS}/libs/dswifi/include" "${BLOCKSDS}/libs/libteak/include" "${BLOCKSDS}/libs/libxm7/include" "${BLOCKSDS}/libs/maxmod/include" "${BLOCKSDS}/libs/libnds/include")
set(ARCH_FLAGS "-march=armv5te -mcpu=arm946e-s+nofp -mtune=arm946e-s")
set(STANDARD_FLAGS  "-ffunction-sections -fdata-sections -D__NDS__ -DARM9")
set(LINKER_FLAGS  "-L${BLOCKSDS}/libs/dswifi/lib -L{BLOCKSDS}/libs/libteak/lib -L${BLOCKSDS}/libs/libxm7/lib -L${BLOCKSDS}/libs/maxmod/lib -L${BLOCKSDS}/libs/libnds/lib")
set(STANDARD_LIBRARIES "-Wl,--start-group -lnds9 -lstdc++ -lc -Wl,--end-group")

if(NDS_DSI_EXCLUSIVE)
    set(LINKER_FLAGS "${LINKER_FLAGS} -specs=${BLOCKSDS}/sys/crts/dsi_arm9.specs")
else()
    set(LINKER_FLAGS "${LINKER_FLAGS} -specs=${BLOCKSDS}/sys/crts/ds_arm9.specs")
endif()

if(NDS_ARCH_THUMB)
    set(ARCH_FLAGS "${ARCH_FLAGS} -mthumb")
endif()


# apply flags
foreach(LANG IN ITEMS C CXX ASM)
    set(CMAKE_${LANG}_FLAGS_INIT "${ARCH_FLAGS} ${STANDARD_FLAGS}")

    set(CMAKE_${LANG}_STANDARD_LIBRARIES "${STANDARD_LIBRARIES}" CACHE STRING "" FORCE)
    set(CMAKE_${LANG}_STANDARD_INCLUDE_DIRECTORIES "${STANDARD_INCLUDE_DIRECTORIES}" CACHE STRING "")
endforeach()

set(CMAKE_ASM_FLAGS_INIT "${CMAKE_ASM_FLAGS_INIT} -x assembler-with-cpp")

set(CMAKE_EXE_LINKER_FLAGS_INIT "${ARCH_FLAGS} ${LINKER_FLAGS}")
