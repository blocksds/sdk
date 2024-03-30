---
title: 'Changelog'
---

## version X.X.X (2024-03-29)

- libnds:

  - Fix C++ builds (there was a missing cast from enum to int, and the Makefile
    was using the wrong program to link binaries).
  - Optimize some videoGl functions (compile them as ARM instead of Thumb to
    take advantage of the faster multiplication instructions).
  - Fix bug in `readdir()`.
  - Implement `__retarget_lock_*()` family of functions to allow libc functions
    to work in a multithreaded environment.
  - Make `glCallList()` take a `void` pointer instead of `u32`.
  - Add checks to NWRAM functions to see if MBK1-MBK5 are writable.
  - Don't remap NWRAM-A when starting the DSP (only B and C are needed).
  - Fix `swiUnpackBits()`.
  - Fix some casts to respect "const".
  - Fix some warnings.
  - Improve some documentation comments.
  - Deprecate typedef `fp` (the name is too short!) and `PUnpackStruct`.
  - Add missing files to the Doxygen documentation.

- SDK:

  - Add examples (3D object picking, 3D toon shading, building 3D display lists,
    3D volumetric shadows, compressed textures, orthogonal projections, using
    console windows with the default console API).
  - Improve C++ test to prevent regressions like during the last few versions.
  - Document memory map of the DS.
  - Add note about the Makefiles of BlocksDS not supporting paths outisde of the
    root folder of the project (thanks, @lifehackerhansol).
  - Fix linking C++ projects in default makefiles.

- maxmod:

  - Fix return type of `mmEffectCancel()` in ARM9 code.

## Version 0.14.0 (2024-03-02)

- libnds:

  - The GRF loading functions have been modified to be actually useful. In some
    cases it wasn't possible to infer the size of some data chunks of the file
    (it was impossible to calculate the size of a tileset, for example). The new
    functions break compatibility with the old ones, but this change was
    required.
  - Added `glTexSizeToEnum()` to convert sizes in pixels to `GL_TEX_SIZE_ENUM`
    values. Also, the funciton `glTexImage2D()` now accepts sizes in pixels as
    well as `GL_TEX_SIZE_ENUM` values.
  - Added a function to return the default drive (`sd:` in the case of DSi,
    `fat:` in the case of a DS).
  - FatFs has been moved to an external repository (wf-fatfs) which is included
    in libnds as a submodule. The documentation of FatFs has been removed from
    this repository.
  - Added some missing 3D polygon attribute definitions.
  - Fixed the return type of `swiSHA1Verify()`.
  - The `fatfs.h` header has been removed, it is redundant.

- SDK:

  - Refactor documentation.
    - It now uses Hugo, and it is available as a static website.
    - Some old sections have been updated.
    - The documentation of all libraries has been integrated with the
      documentation of BlocksDS so that everything is linked.
    - Document how to use Slot-2 flashcards with BlocksDS applications.
    - Reword devkitARM porting guide.
    - A doxygen theme has been applied to the documentation of all the libraries
      used by BlocksDS.

  - Add lots of examples:
    - 3D and 2D graphics.
    - DSWifi.
    - How to use multiple DSP binaries in the same application
    - General NitroFS usage in an application (such as loading music for LibXM7
      or graphics to be used as 2D backgrounds or sprites).
    - DSi SHA1 functions.
    - Hardware timers.
    - Video capture (render to texture, dual screen 3D, save screenshot as PNG).
    - More text console examples.

  - Small change to makefiles that modifies the destination folder of build
    artifacts.

- LibXM7:

  - It now uses timer 0 instead of timer 1, so that Maxmod and LibXM7 use the
    same timer and it's easier to switch libraries.
  - The documentation has been improved.

- DSWifi:

   - WEP modes have been documented.
   - The prototypes of some functions have been cleaned up.
   - The documentation has been improved.

- Maxmod:

   - The return type of `mmEffectCancel()` has been fixed.
   - Some definitions have been turned into enums.
   - `inline` functions in headers have been turned into `static inline.`

## Version 0.13.0 (2024-02-01)

- libnds:

  - Breaking change: Refactor input handling in the ARM9. In order to fix a race
    condition where the touch screen state could be updated between calls to
    `scanKeys()` and `touchRead()` it has become mandatory to call
    `scanKeys()` before `touchRead()`, `keyboardGetChar()`,
    `keyboardUpdate()` and the deprecated `touchReadXY()`. Most programs
    are already doing this, but this may break a small number of programs that
    don't do it.
  - Implemented `isHwDebugger()`, which returns 1 if the console running the
    code is a hardware debugger, regardless of the DS model and CPU state. This
    is meant to replace `swiIsDebugger()`, which only works if the cache is
    disabled, and only in DS models (not DSi). The documentation of
    `swiIsDebugger()` has been updated to mention its limitations.
  - Fix Slot-2 tilt API introduced in version 0.11.0, which hadn't been tested
    on hardware. The old `peripheralSlot2TiltUpdate()` has been replaced by
    `peripheralSlot2TiltStart()` and `peripheralSlot2TiltRead()`.
  - Implemented `utime()` and `utimes()` for changing file modification
    dates.
  - Implemented `scandir()`, `alphasort()` and `versionsort()`,
  - Fixed `statvfs()` and `fstatvfs()` on NitroFS paths and files.
  - Added stubs for `getwd()` and `get_current_dir_name()`.
  - Added stubs for `getuid()`, `getgid()`, etc.
  - Add helpers to load GRF files generated by grit.
  - Reintroduce logic to read Slot-1 cartridges with card commands from the ARM7
    to save CPU cycles on the ARM9.
  - The value of the RAM size field in `REG_SCFG_EXT` in the ARM9 is now set
    to 16 MB or 32 MB instead of being fixed to 32 MB even in retail DSi units.
  - Some CP15 defines have been fixed.
  - Simplify logic in ARM7 input handler.
  - Generate default font from a PNG at build time instead of doing it from a
    preconverted BIN file to make it easier to replace it in the future.

- grit:

  - Added the `-D` argument, specifying the destination folder for
    non-shared data as a counterpart to `-O`.
  - Fixed a situation in which the first color in an image's palette would be
    used as transparent if the user-provided color (with `-gT`) was not
    present in the image.
  - Breaking change: Fixed GRF file output to properly follow the RIFF chunk
    format. It will also export new information in the file header, like using
    special magic values to specify the formats A3I5, A5I3 and TEX4x4, and
    specifying the number of colors contained in the palette (for 16-bit
    textures it's 0). In order for the new information to fit in the header,
    some fields have been increased in size.
  - Fixed palette size calculation for DS textures.
  - Improvements to error messages.

- libxm7:

  - Make types `XM7_XMModuleHeader_Type` and `XM7_MODModuleHeader_Type`
    private. Developers consider their MOD/XM files as a typeless blob of data,
    they shouldn't need to cast it to anything, the library should do it itself.
  - The structs that define the MOD and XM formats have been made private, as
    well as some definitions internal to the player.
  - Some defines have been turned into enums, which will help developers and
    IDEs identify what to use in which functions.
  - The documentation has been updated.

- Tests:

  - Add test to read DSi SCFG registers to see which DSi features are available
    with the loader that has been used to launch the application.
  - Add test to display all configured MPU regions.

## Version 0.12.0 (2023-12-26)

- libnds:

  - Dot and dot-dot entries are now properly emitted in readdir(). In line
    with common software expectations, they mirror standard FAT filesystem
    behaviour, that is are present for all subdirectories. For NitroFS,
    these entries are emulated accordingly.
  - The `d_ino` field in `readdir()` output is now correctly populated, to
    match `stat()` and `fstat()`.
  - Added `nitroFSOpenById()` and `nitroFSFopenById()` functions, allowing
    opening files directly without paying the cost of a directory lookup.
  - Accordingly, NitroFS file systems which contain a FAT table but no FNT
    table can now be opened.
  - Optimized `glMaterialShinyness()`.

- SDK:

  - The default Makefiles have been simplified and now use compiler-provided
    `.specs` files. In turn, a few additional features have been added:
    - Support for picolibc's compiler define-based selection of the printf
      and scanf implementations.
    - The `__BLOCKSDS__` define, which can be used to detect a BlocksDS
      environment during building.
  - Fixed camera initialization with the default ARM7 binary.

- grit:

  - Added the `-ftB` argument, which outputs files with `.img`, `.map`,
    `.meta`, `.pal` extensions, as opposed to `.img.bin`, `.map.bin`,
    `.meta.bin` and `.pal.bin`.

## Version 0.11.3 (2023-12-04)

- libnds:

  - Added helpers to control microphone power independently from recording.
    This can be used for scenarios in which the DSP is tasked from recording
    microphone input.
  - Added helpers and definitions for the DSi GPIO registers.
  - Added function to detect availability of NWRAM.
  - Fixed `atexit()` handlers not being called during a normal `main()`
    return.
  - Fixed TSC configuration for enabling 47 kHz input/output in DSi mode.
  - Improved error handling in Teak DSP code execution helpers.
  - The Teak DSP is now powered off before loading a DSP binary.

- dswifi:

  - Reduced memory usage, especially while Wi-Fi is not initialized.

- ndstool:

  - Breaking: Instead of providing alternate-language banner text using
    `-bt5 "Text"`, the form `-bt 5 "Text"` is now required.
  - Added support for providing mutliple root directories for building NitroFS
    images. All specified root directories are combined to create the root of
    the file system.
  - Fixed `-w` treating other options as file masks.
  - Improved argument handling.

- SDK:

  - Updated compiler flags:
    - The superfluous `-mtune=arm7tdmi` has been removed from ARM7 Makefiles.
    - `-march=armv5te -mtune=arm946e-s` has been replaced with
      `-mcpu=arm946e-s+nofp` in ARM9 Makefiles.
    - `-Wl,--use-blx` has been added to ARM9 linker flags. This allows the
      use of the BLX opcode for linking ARM/Thumb code in place of trampolines,
      slightly improving final executable size and performance.

## Version 0.11.2 (2023-11-27)

- libnds:

  - Fixed a bug introduced in version 0.11.1 that didn't initialize audio
    hardware correctly in DSi mode.
  - Some superfluous audio helpers added in version 0.11.1 have been removed.
  - Move libteak to its own repository so that it can be reused by other
    toolchains.
  - Modify functions to load DSP binaries to return int instead of bool for more
    flexibility.

- DSP:

  - Move crt0 and linkerscript to libteak repository.
  - Preprocess all assembly files, not just the crt0.

## Version 0.11.1 (2023-11-25)

- libnds:

  - Fixed an edge case which could read to invalid small reads/writes to
    DSi/ARM7-controlled removable storage.
  - Added helpers to control `REG_SNDEXTCNT` from the ARM9 (to enable DSP
    audio output to the speakers, for example).
  - Some DSP functions have been moved to `twl` sections to save memory when
    the game runs in a regular DS.
  - Wrapped camera functions to prevent crashes when used in NDS mode.
  - Change license of DLDI-related files to Zlib with permission from the
    authors.
  - Fix Doxygen documentation of peripherals.

- DSP:

  - Added BTDMP helpers to stream audio from the DSP to the speakers. Added an
    example to show how to generate audio from the ARM7 and the DSP at the same
    time.
  - Added an example of how to use DMA to transfer data from the DSP memory to
    the ARM9 memory.

## Version 0.11.0 (2023-11-19)

- libc:

  - Fixed an important regression in `memcpy()` and `memset()` implementations.

- Improved file I/O performance:

  - Added support for batch reads and writes of contiguous clusters, improving
    SD card performance for very large sequential reads/writes.
  - Added `fatInitLookupCacheFile()`. This allows opting a file into having a
    special in-memory cache which significantly speeds up file seek operations.
  - Provisionally automatically enabled the in-memory cache for NitroFS files.
    If you're experiencing slowdowns, make sure to defragment your SD card -
    this requirement will be loosened in future releases (but it's still a good
    idea).
  - Integrated profi200's [dsi\_sdmmc](https://github.com/profi200/dsi_sdmmc)
    driver, improving reliability and performance for reading from and writing
    to the DSi's SD card.
  - Optimized unaligned buffer I/O performance for the DSi's SD card.
  - Only cluster table/directory-related reads will now be cached by the
    built-in sector cache. This allows better use of this sector cache; one can
    use `setvbuf()` to enable a larger cache for file I/O.
  - Other minor optimizations have been made throughout the code.

- Added a new Slot-2 API (`arm9/peripherals/slot2.h`).

  - Added support for detecting external RAM cartridges (SuperCard, M3, G6, DS
    Memory Expansion Pak, EZ-Flash variants, EverDrive).
  - Added support for enabling and disabling the data cache on the Slot-2 memory
    area. Combined with suitable bus speed detection for these cartridges, this
    allows efficient usage of such an external RAM area.
  - Added support for detecting and using the Gyro, Solar and Tilt sensors
    available on various GBA game cartridges.
  - Fixed detection of GBA cartridge rumble (WarioWare, Drill Dozer).
  - Modify rumble example to show how to use the new API.

- DLDI:

  - Moved the built-in sector cache into unused memory occupied by the reserved
    DLDI driver area. This effectly saves ~20KB of heap RAM for most homebrew.
  - The DLDI driver area size can now be changed by defining the `__dldi_size`
    symbol to a value away from the default of `16384`, such as `8192` (if
    your application is highly RAM-constrained - this may break support with
    some cartridges, however) or `32768` (restores compatibility with MoonShell
    versions at the cost of an additional 16KB of RAM).
  - The DLDI driver area is now guaranteed to be close to the beginning of the
    `.nds` file, which may slightly improve load times.

- DSP:

  - Add BTDMP and ICU helpers.
  - Refactor `crt0.s`.
  - Add examples of handling interrupts, including timer interrupts.

- libnds:

  - Rename some cache helpers for consistency.
  - Fixed a file handle leak that could occur if `nitroFSInit()` was pointed to
    an .nds file which does not contain a NitroFS file system.
  - Fixed a rare case in which `nitroFSInit()` could try reading from the GBA
    slot on the DSi, causing an exception.
  - Added `readFirmwareJEDEC()` function to read the ID of the DS firmware flash
    chip. (lifehackerhansol)
  - Minor optimizations have been done to `readUserSettings()`.
  - Fixed the `NDMA_DST_FIX` macro definition.

## Version 0.10.2 (2023-11-11)

- NitroFS: Fix file traversal not working without running a `chdir()` first.

## Version 0.10.1 (2023-11-11)

- Fix NitroFS directory reads occasionally failing on non-DLDI environments.

## Version 0.10.0 (2023-11-11)

- Build system:

  - The default makefiles no longer rely on the toolchain being in the current
    `PATH`. Now, the default Makefiles have the default path to Wonderful
    Toolchain, which can be overriden by the user if desired.
  - The makefiles used by the tests and examples have been moved to a system
    location so that they are more easily reused.
  - dldipatch has replaced dlditool as the default to apply DLDI patches, due to
    bugs in dlditool's patch application process:
    https://problemkaputt.de/gbatek-ds-cart-dldi-driver-guessed-address-adjustments.htm
  - Update your makefiles to take advantage of the updates.

- Filesystem improvements:

  - Replaced NitroFAT by a new Zlib-licensed implementation of the NitroFS
    filesystem by @asiekierka. This fixes the performance drawbacks of NitroFAT.
  - In `stat()` and `fstat()`, the fields `st_dev` and `st_ino` are now
    properly populated.
  - Fixed `stat()` not acknowledging `/` as a directory.
  - Remove NitroFAT support from ndstool.
  - The example makefiles have been modified to stop using mkfatimg.

- DSP:

  - Initial **experimental, incomplete** support for the Teak DSP of the DSi.
    This isn't ready to be used, it's still under development and it's going
    through a lot of changes. Most of the code is derived from @Gericom's
    prototype code.
  - Support for building DSP binaries won't be present on Windows until it's
    more stable. However, if you already have pre-built DSP binaries, it's
    possible to use them on Windows.
  - Introduced teaktool, which converts ELF files into TLF (Teak Loadable
    Format) files that can be loaded by libnds.
  - Added ARM9 functions to libnds to handle the DSP, load TLF files and
    communicate with programs running on the DSP.
  - Introduce libteak, a library with helpers to use the AHBM, DMA, APBP, ICU
    and timer peripherals. It has been documented and added to the Doxygen pages
    of libnds.
  - Added a few examples of how to use the currently supported DSP features.
  - Update user instructions and Dockerfile to use and mention the LLVM Teak
    toolchain.
  - Add NWRAM defintions and helpers.

- DLDI improvements:

  - The DLDI template now automatically calculates the "size" and "fix flags"
    fields of the header.
  - The binary R4 DLDI driver, used for DeSmuMe compatibility, has been replaced
    by a Zlib-licensed impementation built from source.

- libnds:

  - Microphone samples can now be captured using full 16-bit precision on DSi.
  - Cleaned up and added some missing MMIO/bitfield defines throughout libnds.
  - Implemented inlined BIOS calls based on gba-hpp. This should make code
    using BIOS calls slightly smaller and faster.
  - Small reorganization of syscalls code.
  - Slightly optimized coroutine threading code.
  - Added documentation about ARM7 audio helpers.

- Submodules:

  - Before this version, repositories owned by third parties were added as
    submodules to the SDK repository. This can be a problem if the owner isn't
    responsive, changes name, deletes the repository... In order to avoid
    issues, forks have been created under the BlocksDS organization.  It is
    expected to contribute to the original repositories and update the fork to
    stay in sync. Contributing to the forks is a last resort option.

- Tests:

  - Added a new test for SWI functions.

## Version 0.9.1 (2023-10-19)

- Revert changes in maxmod that duplicated some symbols.

## Version 0.9.0 (2023-10-18)

- SDK:

  - Native windows support added. Wonderful toolchains now distribute native
    Windows binaries, and the only required change in BlocksDS was to change a
    library used by Grit. Thank you, Generic and asie!
  - The stdio implementation of picolibc provided by Wonderful Toolchains has
    been patched by asie and this has substantially improved direct SD card read
    and write speeds.
  - The RTC interrupt is no longer used in any test, example or template. Users
    are now expected to timer interrupt instead because the RTC interrupt isn't
    supported on 3DS in DS/DSi mode or most emulators. Check the new code to see
    how to adapt old code. The RTC interrupt functions will still be supported
    to preserve compatibility with old projects that aren't updated.
  - Document the ARM9 <-> ARM7 boot synchronization routine.
  - In the dockerfile, set a locale to be able to pass UTF-8 characters to
    ndstool to appear in the title of the NDS ROM.
  - Add a test to ensure that the libnds modules that use the ARM9 <-> ARM7
    transfer memory region don't break.
  - New examples:
    - Getting key input state.
    - Using NitroFAT, DLDI and DSi SD in the same program.
    - Send a buffer in main RAM to the ARM7 from the ARM9.
    - Read battery status.
    - Set the real time clock of the NDS.

- libnds:

  - RTC:
    - Add new helpers to get and set the date. They use typedefs to move values
      between functions instead of byte arrays.
    - The old helpers that use byte arrays have been deprecated.
    - Using the RTC interrupt as a way to update the time every second has been
      deprecated.
  - Documentation:
    - Document values returned by the battery read function.
    - Document RTC helpers.
    - Add some ARM7 modules to the front page of the Doxygen documentation.
  - Memory:
    - Disable data cache and instruction fetch access to DTCM.
    - Rumble detection functions won't try to detect anything on DSi.
    - Change location of transfer region area on DSi so that it's uncached.
    - Import safe DMA helpers written by Gericom and use them from all DMA
      helpers.
  - Video:
    - Make `glGetInt()` wait for the GPU to be idle when getting the polygon
      and vertices count. It is common for developers to forget to wait.
    - Cleanup some helpers and add some missing VRAM definitions.
    - Document hardware bug of the DMA in GFX FIFO mode.
  - Other:
    - FatFs updated to R0.15p3.
    - Support the debug button (only available in emulators and debug consoles).
    - Switch to using ARM unified syntax (UAL).

- Grit:

  - Switch from libfreeimage to libplum. This allows us to build Grit on Windows
    easier.

## Version 0.8.1 (2023-08-01)

- libnds:

  - Fixed NitroFAT in emulators. It only worked when DLDI was initialized
    correctly, which isn't the case in emulators like no$gba.
  - Set the right CPU as owner of the Slot-1 bus in NitroFAT handling functions.

- SDK:

  - Updated build systems to generate Maxmod soundbanks in the NitroFAT
    filesystem if the filesystem is used. This isn't supported by ARM9 + ARM7
    makefiles for now, only by ARM9 makefiles.
  - Fixed segmentation fault in mkfatimg when not enough arguments are
    provided.
  - Stop relying on `make -j` in Makefiles. It is passed by make to any
    sub-make, so it isn't required.
  - Added basic Maxmod and Maxmod + NitroFAT examples.

## Version 0.8 (2023-07-16)

- libnds:

  - Filesystem:
    - `fatInit()` now correctly sets the current working directory.
    - NitroFAT now changes directory to `nitro:/` on initialization.
    - Fixed code that selects the default filesystem (DSi SD or DLDI).
  - Added asynchronous math functions to suplement the synchronous functions.
  - Added support for redirecting `stdout` and `stderr` to user functions.
  - Added support for more rumble packs.
  - Improved support for DSi regions in `guruMeditationDump()`.
  - Documented MPU setup code properly.
  - Cleaned up exception handling code.
  - Added missing `DLDI_SIZE_2KB` define.
  - Fixed leaking file handlers in `truncate()`.
  - Fixed memory leaks and handling in `image` and `pcx` modules.

- ndstool:

  - Fixed warnings.
  - Removed non-homebrew-related functionality.

- SDK:

  - Automatically link with libc and libstdc++ rather than forcing users to do
    it explicitly.
  - Support `*.arm.c` and `*.arm.cpp` filenames for compatibility with
    devkitARM-utilizing projects.
  - Fixed TLS initialization on the ARM7.
  - Improved bin2c.
  - Updated libc documentation.
  - Improved and cleanup some examples.

## Version 0.7 (2023-04-19)

- libnds:

  - Keyboard:
    - Fixed initialization glitch where it could blink for a frame.
    - Fixed backspace handling.
    - Added support for non-blocking keyboard capture when using cothreads.
  - cothread:
    - Fixed stack alignment.
    - Fixed the stack size of the scheduler thread.
  - Fixed no$gba debug messages on the ARM9.
  - Added support of no$gba debug messages to the ARM7.
  - Implemented `fatInit()`.
  - Improved `sassert()` so that it can exit to the loader instead of locking
    the application.
  - Unified all coding and documentation style of the codebase.
  - Changed license of GL2D to Zlib (with the author's permission).
  - Reduced the size of `OamState` structures.

- mmutil:

  - Fixed segfault with samples with implied zero loop.

- SDK:

  - Improved some old examples. Fix memory leaks in all examples that used
    `getcwd()`.
  - Fixed ARM9 linkerscript to place ITCM sections in ITCM correctly.
  - Added new examples: Exception handling, assertions, no$gba debug console.
  - Prevent mkfatimg from generating FAT images that are so small that FatFs
    can't mount them.
  - Improved installation instructions.

## Version 0.6 (2023-04-11)

- SDK:

  - Added a DLDI driver template.
  - Refactored `install` targets of the SDK components. Now, all components
    can be installed on their own, and they copy the licenses of the components
    to the installation directory.
  - Tweak bin2c behaviour to more closely match devkitPro's bin2s.
  - Use SPDX license identifiers in all libraries and components that end up in
    the NDS application binary.
  - Some cleanup of code formatting.

- **libnds**:

  - FIFO subsystem:
    - The FIFO subsystem has been cleaned up and documented.
    - Some bugs in the FIFO subsystem have been fixed (the stress test still
      fails, though).
    - Prevent using `cothread_yield()` in the ARM7.
  - Alignment of thread local storage sections has been fixed.
  - Added support for calling `stat()` on the root directory of a filesystem.
  - Added support for `statvfs()` and `fstatvfs()`.
  - Avoid pulling in the default keyboard data when stdin-requesting code is
    used. This saves over 10 KB of data in any situation where the default
    keyboard is not used (no keyboard or non-default keyboard alike).
  - Allow setting the duration of the lid sleep check, and to disable it
    completely.
  - Build release versions of the library as well as debug.
  - Document MPU setup steps and CP15 registers.
  - Enable more warnings in the Makefile and fix them.

- ndstool:

  - Support multiple languages in the banner.
  - Support more file formats for icons (GIF, PNG).
  - Support animated icons (from GIF files).

## Version 0.5 (2023-03-31)

- SDK:

  - Defined a default location for BlocksDS: `/opt/blocksds/`
  - libxm7 has been integrated as a core library.
  - Use mkfatimg (distributed with FatFs) instead of `imgbuild.sh` to reduce
    the number of dependencies.
  - Fixed mmutil target in Makefiles in parallel builds.

- libnds:

  - Peripherals:
    - Improved rumble peripheral handling (including detection of the DS Rumble
      Pak).
    - Cleaned up `REG_EXMEMCNT` initialization for the Guitar Grip and Paddle
      peripheral drivers.
  - Improved error recovery in `glInit()`. This allows recovering the
    geometry engine from certain situations where a program exited in the
    middle of 3D engine processing.
  - Improved error recovery in `getcwd()`.
  - Fixed and simplified exit to loader code on the ARM7 side.
  - Improved documentation of exit to loader logic and `BoxTest()`.

## Version 0.4 (2023-03-26)

- SDK:

  - Use the Wonderful Toolchain to get full C++ standard library support.
    - As a result, BlocksDS now targets a specific version of binutils, gcc and
      picolibc.
    - Removed picolibc and avr-libstdcpp as submodules (all previous history has
      been condensed to one commit).
  - Simplified the build system of tests and examples.

- **libnds**:

  - Multithreading:
    - Added cooperative multithreading scheduler.
    - Enabled scheduler in the ARM9 by default.
    - Added examples of having multiple threads, mutexes, and asynchronous file
      loading.
    - Added support for thread-local storage.
    - Added mutexes to FIFO handling and removable storage accesses.
  - Added initial support and example of DSi camera (thanks, asie!).
  - Added support for `malloc()` on the ARM7.
  - Implemented stubs for `fchmod()`, `fchmodat()`, `fchown()`,
    `fchownat()`. `readlink()`, `symlink()`, `getentropy()`.
  - Updated FatFS to R0.15p2.
  - Fixed `glTexImage2D()` not flushing textures before copying them with DMA.

## Version 0.3.1 (2023-03-20)

- libnds:

  - Restored support of `gettimeofday()` on the ARM7.

## Version 0.3 (2023-03-20)

- SDK:

  - Added some tests.
  - Build system improvements (support two-line app titles, remove old makefiles).
  - libsysnds has been integrated in libnds.

- libnds:

  - Implemented a disk cache to improve FatFs performance.
  - Added support for handling DLDI in the ARM7, as opposed to only the ARM9.
    - This is currently controlled either using an additional, previously unused
      bit in the DLDI specification, or explicitly requested by the homebrew
      program.
  - Added function for the ARM9 to request the ARM7 to read the cartridge.
  - Added some missing definitions of DSi registers (SCFG/NDMA).
  - Improved TWL/DSi interrupt support.
  - Improved data cache handling for removable storage read/writes.
  - Fixed detecting certain types of 128 KB cart EEPROMs.
  - Fixed incorrect size detection for certain cases of cart EEPROM data.
  - Tweaked default keyboard texture to make the keycap legends opaque.
  - General cleanup of libnds code (like replacing magic numbers by defines).
  - Fixed `consoleDemoInit()` to restore display brightness when initializing.

## Version 0.2 (2023-03-15)

- SDK:

  - Improved C++ support (now the C++ standard library it is actually usable).
  - Improved C library support.
  - Fixed `install` target.

- libnds:

  - Integrated agbabi renamed as ndsabi. This provides fast implementations of
    `memcpy()`, `memmove()`, `memset()`, helper functions for facilitating
    coroutines, etc.
  - Implemented support for 1BPP fonts in `consoleLoadFont()` and replaced
    `default_font.bin` with a derivative of [Unscii](http://viznut.fi/unscii),
    limited to ASCII characters 32-127. In total, this saves ~7.25 KB of code
    size for any program using the built-in console.
  - Reduced the size of data structures controlling the built-in keyboard.
  - Implemented missing bounds checks in `keyboardGetKey()`.

## Version 0.1 (2023-03-14)

First beta release of BlocksDS. Features:

- SDK:

  - Supports libnds, maxmod, dswifi.
  - Supports a lot of the standard C library.
  - Very early support of the standard C++ library.
  - Supports DLDI, DSi SD slot and NitroFAT (open source alternative of NitroFS)
    using Elm's FatFs.
  - Documentation on how to migrate projects to BlocksDS.
  - Docker image provided.

- libnds:

  - Added new CP15 control helpers for the ARM9.
  - Added Z1/Z2 read support for the TWL/DSi touch screen controller.
    This allows measuring an approximation of pressure, similar to NTR/NDS mode.
