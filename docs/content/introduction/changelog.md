---
title: 'Changelog'
weight: -20
---

## Version 1.3.0 (2024-07-06)

- libnds:

  - New touchscreen driver:

    - The touchscreen driver has been rewritten from the ground up.
    - A new routine for filtering measurements is now used for both TSC (NTR)
      and CDC (TWL) touch inputs. This should provide more accurate results,
      particularly on more worn down displays and screen protectors. More
      testing is required, however.
    - On TSC (NTR), `tscMeasure()` now uses the 16-clock-per-conversion method
      to speed up measurement readouts.
    - The duration of the critical (interrupt-blocking) section of the touch
      driver has been reduced, and the TSC (NTR) driver has had its performance
      optimized on top of that compared to 1.2.0 and below.
    - On the ARM7 side, `touchApplyCalibration` and `touchReadData` have been
      added to allow more granular access to the touchscreen driver's logic.
    - As these commands were only intended for TSC (NTR), `touchRead` and
      `touchReadTemperature` were moved to a new header, `tsc.h`, and renamed
      to `tscRead` and `tscReadTemperature` respectively.

  - Interrupt handling:

    - The interrupt dispatcher has been optimized to use `O(1)` as opposed
      to `O(n)` lookups. This is always faster on ARM9, and faster for
      more than 2-3 defined IRQ handlers on ARM7, which is the common
      scenario.
    - Fixed a bug where `irqClearAUX()` would disable the non-auxillary
      interrupt of the same bit mask on ARM7.
    - Fixed behaviour in setting and clearing of multiple interrupt
      handlers at a time (bit masks with more than one bit set). Now,
      setting multiple bits at once with `irqSet()` or `irqClear()` acts
      the same as setting one bit at a time.

  - Memory usage:

    - ITCM use has been reduced by about 320-380 bytes, depending on your
      codebase's use of libnds.
    - DTCM use has been reduced by 192 bytes - the size of the
      reserved section at the end of memory is now 64 bytes by default
      and can be controlled with the `__dtcm_reserved_size` linker
      symbol.
    - ARM7 IWRAM use has been reduced by 192 bytes - the size of the
      reserved section at the end of memory is now 64 bytes by default
      and can be controlled with the `__iwram_reserved_size` linker
      symbol.
    - The size of the supervisor and IRQ stack can now be controlled by
      defining the `__svc_stack_size` and `__irq_stack_size` linker
      symbols.

  - Graphics:

    - A new function has been added to set the object mode (regular, bitmap,
      window, blended): `oamSetBlendMode()`.
    - A new function has been added to get a SpriteSize entry from the size of a
      sprite in pixels: `oamDimensionsToSize()`. Also, `SpriteSize_Invalid` has
      been introduced to represent dimensions that aren't valid 2D sprite sizes.
    - New definitions have been added for the bit fields of the `BLDALPHA` and
      `BLDY` registers.
    - The following functions now return error codes: `glBindTexture()`,
      `glAssignColorTable()`, `glGetColorTableParameterEXT()`, and
      `glTexParameter()`, making it easier to handle errors gracefully when
      using them.
    - There has been a big refactor in videoGL to handle allocation errors
      gracefully (or at least crash with an assertion if the code can't recover
      from the error).

  - Code refactoring:

    - `RTC_CR`, `RTC_CR8` and `HALT_CR` have been renamed to `REG_RTCCNT`,
      `REG_RTCCNT8` and `REG_HALTCNT`, respectively.
    - GL2D now uses existing `videoGL.h` helpers instead of reimplementing
      its own copies.
    - Many fields and functions have been documented, including firmware
      flash commands, DLDI driver structures.
    - Missing `DMA_START` constants have been added.
    - The constants used in `tscReadTemperature` have been documented.
    - `SerialWaitBusy` has been renamed to `spiWaitBusy`.
    - `oamSetGfx()` has been moved away from the header to fix C++ builds.

  - Other:

    - ARM7 SPI bus helper functions have been added: `spiExchange`,
      `spiRead` and `spiWrite`.
    - `consoleLoadFont()` has been cleaned up. Note that the
      `convertSingleColor` option has been removed, as 1bpp fonts are now
      supported.
    - Decompression of Huffman-compressed data has been implemented.
      To faciliate this, `decompressStreamStruct()` has been added.
    - Decompression utility functions are now available in both the
      ARM9 and ARM7 build of libnds.
    - `glCallList()` and `cardStartTransfer()` now use the safe helper
      function `dmaSetParams()`.
    - wf-fatfs has been updated, bringing minor performance improvements
      to directory lookups.
    - The magic numbers used to represent MPU memory regions have been replaced
      by definitions.
    - A missing include has been added to `grf.h`.

- SDK:

  - New examples:

    - Added an 8-bit bitmap background loading example.
    - Added an example of combining 3D, 2D sprite, and 2D background display.
    - Added an example of loading sprites of all graphics types from GRF files.
    - Added an example of loading bitmap sprites.
    - Added an example of loading and using affine sprites.
    - Added examples of using regular windows and windows using objects as mask.
    - Added examples of using extended palettes for sprites and backgrounds.
    - Added an example of using a text console in an extended affine background.
    - Added an example of using the mosaic effect in sprites and backgrounds.
    - Added an example of animating 2D sprites by updating frames in real time
      or by pre-loading all frames to VRAM from the start.
    - Added an example of using 2D alpha blending and fade effects.
    - Added a BIOS decompression example.
    - Added a NitroFS paletted texture loading example.
    - Added a touch input test, and two examples.
    - Added an example of creating graphics effects using the horizontal
      blanking interrupt.
    - Added a sample minigame based on Space Invaders.

  - Changes to examples:

    - OAM memory and textures is now freed in all examples that allocate them.
      While this isn't needed in examples as short as ours, it's good practice
      to show developers how to free resources when they aren't needed anymore.
    - The names of the ROMs and the titles and subtitles in the ROM header have
      been modified to make them consistent across all examples.
    - Background IDs returned by `bgInit()` are now used instead of hardcoded
      layer numbers.
    - `videoSetMode()` is no longer used to enable sprites or to set sprite
      mapping modes, that should be done by the `libnds` sprite API.
    - Tilemaps are now explicitly excluded from grit generation in all the grit
      files that are used to convert sprites.

  - grit:

    - Fixed reading data from assembly files as input.
    - Fixed uninitialized memory use when loading a picture with an odd width.

  - picolibc:

    - The bundled version of picolibc has been updated.
    - Added an implementation of `funopen`.
    - Added bounds checking to `asctime_r` and `ctime_r`.
    - Added implementations of `fgetpos` and `fsetpos`.
    - Added stub implementations for POSIX unlocked file functions.
    - Fixed `fgetwc` and `fputwc` not setting the file stream to wide
      orientation.
    - Fixed `regex.h` not being usable when compiling C++ code.

  - Other:

    - Fixed NitroFS generation in the combined ARM7+ARM9 ROM template.
    - The BlocksDS SDK now depends on the `wf-nnpack` package, which provides
      standalone, command-line compressors for the decompression methods
      supported by the console's BIOS.
    - The code style of `libteak` and `DSWiFi` has been changed using
      `clang-format` to improve readability.
    - The documentation has been updated with additional notes about migrating
      from older and current versions of devkitARM, covering further potential
      issues.
    - The Teak LLVM toolchain is no longer mentioned in the Windows setup
      instructions, as it is not available there.

## Version 1.2.0 (2024-06-08)

- libnds:

  - videoGL:

    - Add `glGetColorTablePointer()` (returns pointer in VRAM to the palette
      of a texture).
    - Add `glGetTextureExtPointer()` (returns pointer in VRAM to the additional
      texture data of `GL_COMPRESSED` textures).
    - Fix allocation of `GL_COMPRESSED` textures when `VRAM_A` is not allocated
      to the 3D engine.
    - Fix crash in `glTexImage2D()` with no active texture.
    - Fix error checking in `glColorTableEXT()` and `glTexImage2D()`.
    - General code cleanup and improvements.
    - Minor optimization to loading `GL_RGB` textures using `glTexImage2D()`.
      Note that using this format is not recommended; see documentation for
      additional details.
    - `TEXTURE_SIZE_INVALID` has been added to GL_TEXTURE_SIZE_ENUM.
    - `glTexImage2D()` now fails correctly when invalid sizes are used.
    - Some minor comment improvements.

  - GL2D:

    - Improve error checking when loading textures.
    - Modify sprite sets to use `uint16_t` arrays for texture
      coordinates.

  - Other:

    - Add `hw_sqrtf()` - a hardware-accelerated alternative to `sqrtf()`.
    - Small optimizations to functions that use the hardware accelerators of
      division and square root.
    - Add support for detecting stack smash canaries.
      - As a result, the debug versions of libnds are now built with
        the stack protector enabled.
    - Add support for printing standard output to the debug console if
      the on-display console is not initialized.
    - Change `SOUND_FREQ` (ARM7) and `TIMER_FREQ` to always return the
      correct frequency value, rounded to the nearest achievable one.
      - This has been found in user research to be the most intuitive
        default; if you'd like alternate options, please let us know.
    - Fix `swiSwitchToGBAMode()`.
    - Improve documentation of RTC structs.

- SDK:

  - Add error code checks to 3D engine examples.
  - Add GL2D spriteset and tileset examples.
  - Add new tool: squeezer (by @huxingyi), used for generating packed textures
    for GL2D sprite sets. It has been forked to easily export formats used by
    GL2D instead of generic formats.
  - Add tests for `hw_sqrtf()`.
  - Avoid using `GL_RGB` and `TEXTURE_SIZE_n` defines in 3D engine examples.
  - Fix VRAM bank setup in "text over 3D" example.
  - Improve timer example.
  - Add a test for the videoGL functions that allocate textures and palettes.

## Version 1.1.0 (2024-05-08)

- libnds:

  - Add `keyboardExit()` function to deinitialize keyboard structures.
  - Add `realpath()` implementation.
  - Adjust `keyboardInit()` to only link the default keyboard if no custom
    keyboard is being used.
  - Fix `getcwd()` result when called in the root NitroFS directory.
  - Fix Slot-1 card reads not being aligned to 512 bytes.
  - Minor allocation/string handling bugfixes.
  - Fix some memory allocation issues found by GCC 14.

- SDK:

  - Add examples (paletted textures, rotation backgrounds, 16-bit backgrounds,
    sprites in sub screen, FAT file attributes, GL2D usage).
  - Add tests for `realpath()`.
  - Fix warnings found by GCC 14 in examples.

## Version 1.0.0 (2024-03-30)

- libnds:

  - Fix C++ builds (there was a missing cast from enum to int, and the Makefile
    was using the wrong program to link binaries).
  - Optimize some videoGL functions (compile them as ARM instead of Thumb to
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
  - Add note about the Makefiles of BlocksDS not supporting paths outside of the
    root folder of the project (thanks, @lifehackerhansol).
  - Fix linking C++ projects in default makefiles.

- Maxmod:

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
    - Document how to use Slot-2 flashcarts with BlocksDS applications.
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

- LibXM7:

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

- DSWiFi:

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

- Revert changes in Maxmod that duplicated some symbols.

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
  - LibXM7 has been integrated as a core library.
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

  - Supports libnds, Maxmod, DSWiFi.
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
