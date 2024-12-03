# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: ds-sloth, 2024

cmake_minimum_required(VERSION 3.16)

project(portable_demo LANGUAGES C)

set(PORTABLE_DEMO_SRC
    src/cross_platform_logic.c
)

if(NINTENDO_DS)
    LIST(APPEND PORTABLE_DEMO_SRC
        src/nds/main_nds.c
    )
else()
    LIST(APPEND PORTABLE_DEMO_SRC
        src/posix/main_posix.c
    )
endif()

add_executable(portable_demo ${PORTABLE_DEMO_SRC})

target_include_directories(portable_demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(NINTENDO_DS)
    if(NDS_DSI_EXCLUSIVE)
        set(SUBTITLE "DSi Edition")
    else()
        set(SUBTITLE "Same logic everywhere")
    endif()

    nds_create_rom(portable_demo
        NAME "Portable Demo"
        SUBTITLE "${SUBTITLE}"
        AUTHOR "ds-sloth"
    )
endif()
