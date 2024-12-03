## BlocksDS CMake

BlocksDS provides simple CMake build scripts for cross-platform projects. These build scripts allow developers to build their applications for the DS/DSi and create a rom using the default ARM7 binary.

### Usage

Add `-DCMAKE_TOOLCHAIN_FILE=$BLOCKSDS/cmake/BlocksDS.cmake` (or `BlocksDSi.cmake`) to your `cmake` invocation to set up your project to build an ARM9 binary for the DS(i).

This will define CMake variables `BLOCKSDS` (a path to the BLOCKSDS core directory), `NINTENDO_DS`, and (possibly) `NDS_DSI_EXCLUSIVE`. Your project may make use of these variables to add NDS-specific source files to your build.

#### nds_create_rom
The toolchain file provides a new CMake function `nds_create_rom`. This takes your main executable target as well as arguments specifying the NDS header. All arguments are optional. An example:

```
nds_create_rom(my_target
    NAME "My Cool Game"
    SUBTITLE "An Example Project"
    AUTHOR "Me, Myself, and I"
    ICON "${CMAKE_CURRENT_SOURCE_DIR}/nds-icon.bmp"
    ARM7 "${CMAKE_CURRENT_SOURCE_DIR}/arm7.elf"
    NITROFS "${CMAKE_CURRENT_SOURCE_DIR}/nitrofs-dir"
)
```

#### Toolchain file selection
The only difference between `BlocksDS.cmake` and `BlocksDSi.cmake` is that the latter allows the entire memory space of the DSi to be used by the main program's code and statically-allocated variables. Otherwise, only ~3.5MB are available. Note that including an ARM9 binary above 2.5MB violates the NDS ROM standard and such roms may fail to boot in some menus.

#### Toolchain variables
Currently the BlocksDS toolchain file accepts the following variables. They should be set in the initial `cmake` invocation. 

* `NDS_ARCH_THUMB`: if this flag is set, then all code will be generated for the thumb instruction set.

The toolchain file also checks the local environment variables `BLOCKSDS` to locate the BlocksDS core directory and `WONDERFUL_TOOLCHAIN` to locate the Wonderful root directory.