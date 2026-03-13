# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: ds-sloth, 2024
# SPDX-FileContributor: trustytrojan, 2026


cmake_minimum_required(VERSION 3.16)



if(NOT CMAKE_SYSTEM_NAME)
    # Note: this will cause Platform/NintendoDS.cmake to be loaded at the correct stage.
    set(CMAKE_SYSTEM_NAME NintendoDS)
endif()

if(NOT CMAKE_SYSTEM_VERSION)
    set(CMAKE_SYSTEM_VERSION 1)
endif()

if(NOT CMAKE_SYSTEM_PROCESSOR)
    set(CMAKE_SYSTEM_PROCESSOR armv5te)
endif()

if(CMAKE_HOST_WIN32)
    message(FATAL_ERROR "The BlocksDS CMake toolchain requires a posix-like environment (mwingw, msys, etc)")
endif()

if(NOT "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "armv5te")
    message(FATAL_ERROR "This CMake toolchain can only produce binaries for armv5te. (CMAKE_SYSTEM_PROCESSOR [${CMAKE_SYSTEM_PROCESSOR}] requested).")
endif()

set(NINTENDO_DS TRUE)
set(ARM9 TRUE)



# initialize basic platform properties
list(APPEND CMAKE_TRY_COMPILE_PLATFORM_VARIABLES NDS_ARCH_THUMB)
list(APPEND CMAKE_TRY_COMPILE_PLATFORM_VARIABLES NDS_DSI_EXCLUSIVE)

if(NOT CMAKE_USER_MAKE_RULES_OVERRIDE)
    set(CMAKE_USER_MAKE_RULES_OVERRIDE ${CMAKE_CURRENT_LIST_DIR}/BlocksDS-rule-overrides.cmake)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")



# find WONDERFUL_TOOLCHAIN and BLOCKSDS
if(NOT DEFINED ENV{WONDERFUL_TOOLCHAIN})
    message(FATAL_ERROR "Ensure that environment variable WONDERFUL_TOOLCHAIN is set.")
else()
    set(WONDERFUL_TOOLCHAIN "$ENV{WONDERFUL_TOOLCHAIN}")
endif()

if(NOT DEFINED ENV{BLOCKSDS})
    if(EXISTS "/opt/blocksds/core/")
        set(BLOCKSDS "/opt/blocksds/core")
    elseif(EXISTS "${WONDERFUL_TOOLCHAIN}/thirdparty/blocksds/core/")
        set(BLOCKSDS "${WONDERFUL_TOOLCHAIN}/thirdparty/blocksds/core")
    else()
        message(FATAL_ERROR "BlocksDS not found. Ensure that environment variable BLOCKSDS is set.")
    endif()
else()
    set(BLOCKSDS "$ENV{BLOCKSDS}")
endif()

if(NOT DEFINED ENV{BLOCKSDSEXT})
    if(EXISTS "/opt/blocksds/external/")
        set(BLOCKSDSEXT "/opt/blocksds/external")
    elseif(EXISTS "${WONDERFUL_TOOLCHAIN}/thirdparty/blocksds/external/")
        set(BLOCKSDSEXT "${WONDERFUL_TOOLCHAIN}/thirdparty/blocksds/external")
    else()
        set(BLOCKSDSEXT "")
    endif()
else()
    set(BLOCKSDSEXT "$ENV{BLOCKSDSEXT}")
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${BLOCKSDSEXT}" CACHE PATH
        "Install path prefix, prepended onto install directories." FORCE)
endif()



# initialize search paths and executables for the desired triplet
set(BLOCKSDS_TRIPLET "arm-none-eabi")

# Prefer config mode for find_package (matches dkP logic)
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)

set(CMAKE_FIND_USE_SYSTEM_ENVIRONMENT_PATH OFF)
string(REPLACE ":" ";" CMAKE_SYSTEM_PROGRAM_PATH "$ENV{PATH}")

set(CMAKE_SYSTEM_PREFIX_PATH
    ${BLOCKSDSEXT}
    ${BLOCKSDS}/libs/dswifi
    ${BLOCKSDS}/libs/libteak
    ${BLOCKSDS}/libs/libxm7
    ${BLOCKSDS}/libs/maxmod
    ${BLOCKSDS}/libs/libnds
    ${WONDERFUL_TOOLCHAIN}/toolchain/gcc-${BLOCKSDS_TRIPLET}
    ${WONDERFUL_TOOLCHAIN}/toolchain/gcc-${BLOCKSDS_TRIPLET}/${BLOCKSDS_TRIPLET}
    ${BLOCKSDS}/tools/ndstool
)

set(TOOLCHAIN_PATH_HINT ${WONDERFUL_TOOLCHAIN}/toolchain/gcc-${BLOCKSDS_TRIPLET}/bin)
find_program(CMAKE_ASM_COMPILER ${BLOCKSDS_TRIPLET}-gcc        HINTS ${TOOLCHAIN_PATH_HINT})
find_program(CMAKE_C_COMPILER   ${BLOCKSDS_TRIPLET}-gcc        HINTS ${TOOLCHAIN_PATH_HINT})
find_program(CMAKE_CXX_COMPILER ${BLOCKSDS_TRIPLET}-g++        HINTS ${TOOLCHAIN_PATH_HINT})
find_program(CMAKE_LINKER       ${BLOCKSDS_TRIPLET}-ld         HINTS ${TOOLCHAIN_PATH_HINT})
find_program(CMAKE_AR           ${BLOCKSDS_TRIPLET}-gcc-ar     HINTS ${TOOLCHAIN_PATH_HINT})
find_program(CMAKE_RANLIB       ${BLOCKSDS_TRIPLET}-gcc-ranlib HINTS ${TOOLCHAIN_PATH_HINT})
find_program(CMAKE_STRIP        ${BLOCKSDS_TRIPLET}-strip      HINTS ${TOOLCHAIN_PATH_HINT})

find_program(BLOCKSDS_NDSTOOL NAMES ndstool HINTS "${BLOCKSDS}/tools/ndstool")
find_program(BLOCKSDS_DSLTOOL NAMES dsltool HINTS "${BLOCKSDS}/tools/dsltool")
find_file(BLOCKSDS_ARM9_DSL_SPECS NAMES ds_arm9_dsl.specs HINTS "$ENV{BLOCKSDS}/sys/crts")


# post-link utilities to create NDS rom
function(nds_create_rom target)
    cmake_parse_arguments(PARSE_ARGV 1 NDSTOOL "" "OUTPUT;ARM9;ARM7;NAME;SUBTITLE;AUTHOR;ICON;NITROFS;SUBTITLE1;SUBTITLE2" "FLAGS")

    if(NOT BLOCKSDS_NDSTOOL)
        message(FATAL_ERROR "Could not find ndstool: try installing ndstool")
    endif()

    if(NOT TARGET "${target}")
        message(FATAL_ERROR "nds_create_rom: target '${target}' not defined")
    endif()

    if(DEFINED NDSTOOL_ARM9)
        message(FATAL_ERROR "nds_create_rom: custom ARM9 elf not supported")
    endif()


    # resolve output rom filename
    get_target_property(target_dir ${target} BINARY_DIR)
    get_target_property(target_filename ${target} OUTPUT_NAME)
    if(NOT target_filename)
        set(target_filename "${target}")
    endif()

    set(NDSTOOL_OUTPUT "${target_dir}/${target_filename}.nds")


    # resolve input ARM7 elf file
    if(NOT DEFINED NDSTOOL_ARM7)
        set(NDSTOOL_ARM7 "${BLOCKSDS}/sys/default_arm7/arm7.elf")
    endif()


    # resolve details (NAME/SUBTITLE/AUTHOR) for ndstool
    if (NOT DEFINED NDSTOOL_NAME)
        set(NDSTOOL_NAME "${CMAKE_PROJECT_NAME}")
    endif()
    if (NOT DEFINED NDSTOOL_SUBTITLE AND DEFINED NDSTOOL_SUBTITLE1)
        set(NDSTOOL_SUBTITLE "${NDSTOOL_SUBTITLE1}")
    endif()
    if (NOT DEFINED NDSTOOL_AUTHOR AND DEFINED NDSTOOL_SUBTITLE2)
        set(NDSTOOL_AUTHOR "${NDSTOOL_SUBTITLE2}")
    endif()

    if (NOT DEFINED NDSTOOL_SUBTITLE AND NOT DEFINED NDSTOOL_AUTHOR)
        set(NDSTOOL_FULL_TITLE "${NDSTOOL_NAME}\;Built with BlocksDS\;blocksds.skylyrac.net")
    else()
        set(NDSTOOL_FULL_TITLE "${NDSTOOL_NAME}")
        if(DEFINED NDSTOOL_SUBTITLE)
            set(NDSTOOL_FULL_TITLE "${NDSTOOL_FULL_TITLE}\;${NDSTOOL_SUBTITLE}")
        endif()
        if(DEFINED NDSTOOL_AUTHOR)
            set(NDSTOOL_FULL_TITLE "${NDSTOOL_FULL_TITLE}\;${NDSTOOL_AUTHOR}")
        endif()
    endif()


    # resolve icon
    if (NOT DEFINED NDSTOOL_ICON)
        find_file(NDSTOOL_ICON NAMES icon.gif HINTS "${BLOCKSDS}/sys")

        if(NOT NDSTOOL_ICON)
            message(FATAL_ERROR "nds_create_rom: could not find default icon, try installing libnds")
        endif()
    elseif(TARGET "${NDSTOOL_ICON}")
        message(FATAL_ERROR "nds_create_rom: ICON must be a file and not a target")
    else()
        get_filename_component(NDSTOOL_ICON "${NDSTOOL_ICON}" ABSOLUTE)

        if(NOT EXISTS "${NDSTOOL_ICON}")
            message(FATAL_ERROR "nds_create_rom: cannot find ICON [${NDSTOOL_ICON}]")
        endif()
    endif()


    # prepare ndstool invocation
    set(NDSTOOL_DEPS ${target} "${NDSTOOL_ARM7}")

    set(NDSTOOL_ARGS
        -c "${NDSTOOL_OUTPUT}"
        -9 "$<TARGET_FILE:${target}>"
        -7 "${NDSTOOL_ARM7}"
        -b "${NDSTOOL_ICON}"
        "${NDSTOOL_FULL_TITLE}")

    if (DEFINED NDSTOOL_FLAGS)
        list(APPEND NDSTOOL_ARGS ${NDSTOOL_FLAGS})
    endif()


    # add nitrofs if present
    if (DEFINED NDSTOOL_NITROFS)
        get_filename_component(NDSTOOL_NITROFS "${NDSTOOL_NITROFS}" ABSOLUTE)

        if (NOT IS_DIRECTORY "${NDSTOOL_NITROFS}")
            message(FATAL_ERROR "nds_create_rom: NITROFS must be a directory [${NDSTOOL_NITROFS}]")
        endif()

        list(APPEND NDSTOOL_ARGS -d "${NDSTOOL_NITROFS}")
    endif()


    # invoke ndstool
    add_custom_command(
        OUTPUT "${NDSTOOL_OUTPUT}"
        COMMAND "${BLOCKSDS_NDSTOOL}" ${NDSTOOL_ARGS}
        DEPENDS ${target} "${NDSTOOL_ARM7}"
        COMMENT "Building NDS ROM target ${outtarget}"
        VERBATIM
    )

    add_custom_target(${target}_nds ALL DEPENDS "${NDSTOOL_OUTPUT}")
endfunction()

# Utility function to create a DSL library using a static library file built by a CMake STATIC library target.
#
# Required:
# - DSL_TARGET: an existing STATIC library target name used as linker input for the intermediary ELF
#
# Optional:
# - MAIN_TARGET: if provided, uses the ELF of this CMake target for dsltool's -m option
# - VERBOSE_OUTPUT: if set, passes -v to dsltool
# - IGNORE_UNRESOLVED_SYMBOLS: if set, passes -u to dsltool
#
# Outputs:
# - ${TARGET}_ELF: path to the ELF file
# - ${TARGET}_DSL: path to the DSL file
# - ${TARGET}_DSL_TARGET: name of the target that builds the DSL file
function(blocksds_create_dsl DSL_TARGET)
    set(options VERBOSE_OUTPUT IGNORE_UNRESOLVED_SYMBOLS)
	set(oneValueArgs TARGET MAIN_TARGET)
	cmake_parse_arguments(CREATE_DSL "${options}" "${oneValueArgs}" "" ${ARGN})

    ## Verify passed arguments.
	if(NOT DSL_TARGET)
		message(FATAL_ERROR "TARGET is required")
	endif()

	if(NOT TARGET ${DSL_TARGET})
		message(FATAL_ERROR "TARGET '${DSL_TARGET}' does not exist")
	endif()

	get_target_property(_static_target_type ${DSL_TARGET} TYPE)
	if(NOT _static_target_type STREQUAL "STATIC_LIBRARY")
		message(FATAL_ERROR "TARGET '${DSL_TARGET}' must be a STATIC library target")
	endif()

    # Setup output target name and output file paths.
	set(_basename "${DSL_TARGET}")
	set(_dsl_target "${DSL_TARGET}_dsl")
	set(_elf "${CMAKE_BINARY_DIR}/${_basename}.elf")
	set(_dsl "${CMAKE_BINARY_DIR}/${_basename}.dsl")

    # Sanity check the ARM9 DSL specs file
    if(NOT EXISTS ${BLOCKSDS_ARM9_DSL_SPECS})
        message(FATAL_ERROR "ds_arm9_dsl.specs not found! Your CMake toolchain file may be broken.")
    endif()

	## Create a command to link the static library into an ELF for dsltool.
    # We use CMAKE_CXX_COMPILER as the "linker driver" because g++ will handle both C and C++ linking quirks.
	add_custom_command(
		OUTPUT ${_elf}
		COMMAND ${CMAKE_CXX_COMPILER}
			-mthumb
			-mcpu=arm946e-s+nofp
			-nostdlib
			-specs=${BLOCKSDS_ARM9_DSL_SPECS}
			-Wl,--emit-relocs
			-Wl,--unresolved-symbols=ignore-all
			-Wl,--nmagic # this removes .text/.data section padding... good
			-Wl,--target1-abs
			-Wl,--whole-archive
			$<TARGET_FILE:${DSL_TARGET}>
            -Wl,--no-whole-archive # probably not necessary
			-o ${_elf}
		DEPENDS ${DSL_TARGET}
		VERBATIM
	)

    ## Arguments passed to dsltool.
	set(_dsltool_args -i ${_elf} -o ${_dsl})

    if(CREATE_DSL_VERBOSE_OUTPUT)
        list(APPEND _dsltool_args -v)
    endif()

    if(CREATE_DSL_IGNORE_UNRESOLVED_SYMBOLS)
        list(APPEND _dsltool_args -u)
    endif()

    ## Dependencies of the dsltool command.
    # For now, start with the ELF's existence.
	set(_dsltool_deps ${_elf})
	
	# MAIN_TARGET is optional.
	# Use it when you want dsltool to resolve symbols against the main binary at build-time.
	if(CREATE_DSL_MAIN_TARGET)
		if(NOT TARGET ${CREATE_DSL_MAIN_TARGET})
			message(FATAL_ERROR "MAIN_TARGET '${CREATE_DSL_MAIN_TARGET}' does not exist")
		endif()

		list(APPEND _dsltool_args -m $<TARGET_FILE:${CREATE_DSL_MAIN_TARGET}>)

        # Ensure that MAIN_TARGET builds before running dsltool.
		list(APPEND _dsltool_deps ${CREATE_DSL_MAIN_TARGET})
	endif()

    # Sanity check for dsltool's existence
    if(NOT EXISTS "${BLOCKSDS_DSLTOOL}")
        message(FATAL_ERROR "dsltool not found! Your CMake toolchain file may be broken.")
    endif()

    ## Create the dsltool command.
	add_custom_command(
		OUTPUT ${_dsl}
		COMMAND ${BLOCKSDS_DSLTOOL} ${_dsltool_args}
		DEPENDS ${_dsltool_deps}
		VERBATIM
	)

	## Create a target containing the above commands that create our DSL.
    # Add it to the ALL target to build it at the same time as other
    # libraries/executables, giving the caller no extra work to do.
	add_custom_target(${_dsl_target} ALL DEPENDS ${_dsl})

    ## Return convenient values to the caller.
    # Path to the ELF file used to create the DSL.
	set(${DSL_TARGET}_ELF ${_elf} PARENT_SCOPE)
    # Path to the DSL file.
	set(${DSL_TARGET}_DSL ${_dsl} PARENT_SCOPE)
    # Name of the DSL target.
	set(${DSL_TARGET}_DSL_TARGET ${_dsl_target} PARENT_SCOPE)
endfunction()
