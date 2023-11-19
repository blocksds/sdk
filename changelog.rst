##################
BlocksDS changelog
##################

Version 0.11.0 (2023-11-19)
===========================

libc:

  - Fixed an important regression in ``memcpy()`` and ``memset()``
    implementations.

- Improved file I/O performance:

  - Added support for batch reads and writes of contiguous clusters, improving
    SD card performance for very large sequential reads/writes.
  - Added ``fatInitLookupCacheFile()``. This allows opting a file into having a
    special in-memory cache which significantly speeds up file seek operations.
  - Provisionally automatically enabled the in-memory cache for NitroFS files.
    If you're experiencing slowdowns, make sure to defragment your SD card -
    this requirement will be loosened in future releases (but it's still a good
    idea).
  - Integrated profi200's `dsi_sdmmc <https://github.com/profi200/dsi_sdmmc>`_
    driver, improving reliability and performance for reading from and writing
    to the DSi's SD card.
  - Optimized unaligned buffer I/O performance for the DSi's SD card.
  - Only cluster table/directory-related reads will now be cached by the
    built-in sector cache. This allows better use of this sector cache; one can
    use ``setvbuf()`` to enable a larger cache for file I/O.
  - Other minor optimizations have been made throughout the code.

- Added a new Slot-2 API (``arm9/peripherals/slot2.h``).

  - Added support for detecting external RAM cartridges (SuperCard, M3, G6, DS
    Memory Expansion Pak, EZ-Flash variants, EverDrive).
  - Added support for enabling and disabling the data cache on the Slot-2 memory
    area. Combined with suitable bus speed detection for these cartridges, this
    allows efficient usage of such an external RAM area.
  - Added support for detecting and using the Gyro, Solar and Tilt sensors
    available on various GBA game cartridges.
  - Fixed detection of GBA cartridge rumble (WarioWare, Drill Dozer).
  - Modify rumble example to show how to use the new API.

DLDI:

  - Moved the built-in sector cache into unused memory occupied by the reserved
    DLDI driver area. This effectly saves ~20KB of heap RAM for most homebrew.
  - The DLDI driver area size can now be changed by defining the ``__dldi_size``
    symbol to a value away from the default of ``16384``, such as ``8192`` (if
    your application is highly RAM-constrained - this may break support with
    some cartridges, however) or ``32768`` (restores compatibility with
    MoonShell versions at the cost of an additional 16KB of RAM).
  - The DLDI driver area is now guaranteed to be close to the beginning of the
    .nds file, which may slightly improve load times.

- DSP:

  - Add BTDMP and ICU helpers.
  - Refactor ``crt0.s``.
  - Add examples of handling interrupts, including timer interrupts.

- ``libnds``:

  - Rename some cache helpers for consistency.
  - Fixed a file handle leak that could occur if ``nitroFSInit()`` was pointed
    to an .nds file which does not contain a NitroFS file system.
  - Fixed a rare case in which ``nitroFSInit()`` could try reading from the GBA
    slot on the DSi, causing an exception.
  - Added ``readFirmwareJEDEC()`` function to read the ID of the DS firmware
    flash chip. (lifehackerhansol)
  - Minor optimizations have been done to ``readUserSettings()``.
  - Fixed the ``NDMA_DST_FIX`` macro definition.

Version 0.10.2 (2023-11-11)
===========================

- NitroFS: Fix file traversal not working without running a ``chdir()`` first.

Version 0.10.1 (2023-11-11)
===========================

- Fix NitroFS directory reads occasionally failing on non-DLDI environments.

Version 0.10.0 (2023-11-11)
===========================

- Build system:

  - The default makefiles no longer rely on the toolchain being in the current
    ``PATH``. Now, the default Makefiles have the default path to Wonderful
    Toolchain, which can be overriden by the user if desired.
  - The makefiles used by the tests and examples have been moved to a system
    location so that they are more easily reused.
  - ``dldipatch`` has replaced ``dlditool`` as the default to apply DLDI
    patches, due to bugs in dlditool's patch application process:
    https://problemkaputt.de/gbatek-ds-cart-dldi-driver-guessed-address-adjustments.htm
  - Update your makefiles to take advantage of the updates.

- Filesystem improvements:

  - Replaced NitroFAT by a new Zlib-licensed implementation of the NitroFS
    filesystem by @asiekierka. This fixes the performance drawbacks of NitroFAT.
  - In ``stat()`` and ``fstat()``, the fields ``st_dev`` and ``st_ino`` are now
    properly populated.
  - Fixed ``stat()`` not acknowledging ``/`` as a directory.
  - Remove NitroFAT support from ``ndstool``.
  - The example makefiles have been modified to stop using ``mkfatimg``.

- DSP:

  - Initial **experimental, incomplete** support for the Teak DSP of the DSi.
    This isn't ready to be used, it's still under development and it's going
    through a lot of changes. Most of the code is derived from @Gericom's
    prototype code.
  - Support for building DSP binaries won't be present on Windows until it's
    more stable. However, if you already have pre-built DSP binaries, it's
    possible to use them on Windows.
  - Introduced ``teaktool``, which converts ELF files into TLF (Teak Loadable
    Format) files that can be loaded by ``libnds``.
  - Added ARM9 functions to ``libnds`` to handle the DSP, load TLF files and
    communicate with programs running on the DSP.
  - Introduce ``libteak``, a library with helpers to use the AHBM, DMA, APBP,
    ICU and timer peripherals. It has been documented and added to the Doxygen
    pages of ``libnds``.
  - Added a few examples of how to use the currently supported DSP features.
  - Update user instructions and Dockerfile to use and mention the LLVM Teak
    toolchain.
  - Add NWRAM defintions and helpers.

- DLDI improvements:

  - The DLDI template now automatically calculates the "size" and "fix flags"
    fields of the header.
  - The binary R4 DLDI driver, used for DeSmuMe compatibility, has been replaced
    by a Zlib-licensed impementation built from source.

- ``libnds``:

  - Microphone samples can now be captured using full 16-bit precision on DSi.
  - Cleaned up and added some missing MMIO/bitfield defines throughout libnds.
  - Implemented inlined BIOS calls based on ``gba-hpp``. This should make code
    using BIOS calls slightly smaller and faster.
  - Small reorganization of syscalls code.
  - Slightly optimized coroutine threading code.
  - Added documentation about ARM7 audio helpers.

- Submodules:

  - Before this version, repositories owned by third parties were added as
    submodules to the SDK repository. This can be a problem if the owner isn't
    responsive, changes name, deletes the repository... In order to avoid
    issues, forks have been created under the BlocksDS organization.
    It is expected to contribute to the original repositories and update the
    fork to stay in sync. Contributing to the forks is a last resort option.

- Tests:

  - Added a new test for SWI functions.

Version 0.9.1 (2023-10-19)
==========================

- Revert changes in maxmod that duplicated some symbols.

Version 0.9.0 (2023-10-18)
==========================

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

- ``libnds``:

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

    - Make ``glGetInt()`` wait for the GPU to be idle when getting the polygon
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

Version 0.8.1 (2023-08-01)
==========================

- ``libnds``:

  - Fix NitroFAT in emulators. It only worked when DLDI was initialized
    correctly, which isn't the case in emulators like no$gba.
  - Set the right CPU as owner of the Slot-1 bus in NitroFAT handling functions.

- SDK:

  - Update build systems to generate Maxmod soundbanks in the NitroFAT
    filesystem if the filesystem is used. This isn't supported by ARM9 + ARM7
    makefiles for now, only by ARM9 makefiles.
  - Fix segmentation fault in ``mkfatimg`` when not enough arguments are
    provided.
  - Stop relying on ``make -j`` in Makefiles. It is passed by make to any
    sub-make, so it isn't required.
  - Add basic Maxmod and Maxmod + NitroFAT examples.

Version 0.8 (2023-07-16)
========================

- ``libnds``:

  - Document MPU setup code properly.
  - Cleanup exception handling code.
  - Add asynchronous math functions to complement the previous synchronous ones.
  - Support redirecting ``stdout`` and ``stderr`` to user functions.
  - Fix code that selects the default filesystem (DSi SD or DLDI).
  - Fix leaking file handlers in ``truncate()``.
  - Fix memory leaks in ``image`` and ``pcx`` modules.
  - Support more rumble packs.

- ``ndstool``:

  - Fix warnings.
  - Remove non-homebrew-related functionality.

- SDK:

  - Automatically link with libc and libstdc++ rather than forcing users to do
    it explicitly.
  - Support ``*.arm.c`` and ``*.arm.cpp`` filenames for compatibility with
    devkitARM.
  - Correctly initialize TLS in the ARM7.
  - Improve ``bin2c``.
  - Update libc documentation.
  - Improve and cleanup some examples.

Version 0.7 (2023-04-19)
========================

- ``libnds``:

  - Unify all coding and documentation style of the codebase.
  - Change license of GL2D to Zlib (with the author's permission).
  - Improve ``sassert()`` so that it can exit to the loader instead of locking
    the application.

  - Keyboard:

    - Fix initialization glitch where it would blink for a frame.
    - Fix backspace handling.
    - Make it use cothread functions so that it never blocks the application.

  - ``cothread``:

    - Fixed stack alignment
    - Fixed stack size of the scheduler thread.

  - ``libc``:

    - Fix no$gba debug messages on the ARM9.
    - Add support of no$gba debug messages to the ARM7.

- ``mmutil``:

  - Fix segfault with samples with implied zero loop.

- SDK:

  - Improve some old examples. Fix memory leaks in all examples that used
    ``getcwd()``.
  - Fix ARM9 linkerscript to place ITCM sections in ITCM correctly.
  - Add new examples: Exception handling, assertions, no$gba debug console.
  - Prevent ``mkfatimg`` from generating FAT images that are so small that FatFs
    can't mount them.
  - Improve installation instructions.

Version 0.6 (2023-04-11)
========================

- ``libnds``:

  - Document MPU setup steps and CP15 registers.
  - Enable more warnings in the Makefile and fix them.

  - FIFO subsystem:

    - The FIFO subsystem has been cleaned up and documented.
    - Some bugs in the FIFO subsystem have been fixed (the stress test still
      fails, though).
    - Prevent using ``cothread_yield()`` in the ARM7.

  - ``libc``:

    - Alignment of thread local storage sections has been fixed.
    - Support ``stat()`` in the root directory of a filesystem.
    - Support ``statvfs()`` and ``fstatvfs()``.

  - Avoid including the default keyboard data if it isn't used.
  - Allow setting the duration of the lid sleep check, and to disable it
    completely.
  - Build release versions of the library as well as debug.

- ``ndstool``:

  - Support multiple languages in the banner.
  - Support more file formats for icons (GIF, PNG).
  - Support animated icons (from GIF files).

- SDK:

  - Refactor ``install`` targets of the SDK components. Now, all components can
    be installed on their own, and they copy the licenses of the components to 
    the installation directory.
  - Emulate behaviour of ``bin2s`` of devkitPro more closely with ``bin2c``.
  - Use SPDX license identifiers in all libraries and components that end up in
    the NDS application binary.
  - Some cleanup of code formatting.
  - Added a DLDI driver template.

Version 0.5 (2023-03-31)
========================

- Define a default location for BlocksDS: ``/opt/blocksds/``
- Use ``mkfatimg`` (distributed with FatFs) instead of ``imgbuild.sh`` to reduce
  the number of dependencies.
- Fix ``mmutil`` target in Makefiles in parallel builds.
- Cleanup of GBA slot peripherals drivers.
- Integrate ``libxm7`` as a core library.
- Fix exit to loader code from the ARM7. It has been simplified a bit.
- Document exit to loader code.

Version 0.4 (2023-03-26)
========================

- Use Wonderful Toolchains to get full C++ standard library support.
- Remove ``picolibc`` and ``avr-libstdcpp`` as submodules (all previous history
  has been condensed to one commit).
- Multithreading:

  - Add cooperative multithreading scheduler.
  - Enable scheduler in the ARM9 by default.
  - Add examples of having multiple threads, mutexes, and asynchronous file
    loading.
  - Support thread local storage.
  - Added mutexes to problematic parts of ``libnds`` and FatFs.

- Add initial support and example of DSi camera (thanks, asie!).
- Support ``malloc()`` in the ARM7.
- Simplify build system of tests and examples.
- Bugfixes.

Version 0.3.1 (2023-03-20)
==========================

- Hotfix.

Version 0.3 (2023-03-20)
========================

- FatFs performance improvements (like adding a disk cache).
- Support DLDI in the ARM7 as well as the ARM9.
- Add function for the ARM9 to request the ARM7 to read the cartridge.
- Add some missing definitions of DSi registers (SCFG/NDMA).
- General cleanup of ``libnds`` code (like replacing magic numbers by defines).
- Build system improvements (support two line app titles, remove old makefiles).
- ``libsysnds`` has been integrated in ``libnds``.
- Bugfixes in libc and ``libnds``.

  - EEPROM handling functions.
  - Data cache handling bugs.
  - Fix transparency in keyboard of ``libnds``.

- Added some tests.

Version 0.2 (2023-03-15)
========================

- Improve C++ support (now the C++ standard library it is actually usable).
- Improve C library support.
- Integrate agbabi as ``ndsabi`` (provides fast ``memcpy``, coroutines, etc).
- Fix ``install`` target.

Version 0.1 (2023-03-14)
========================

First beta release of BlocksDS. Features:

- Supports ``libnds``, ``maxmod``, ``dswifi``.
- Supports a lot of the standard C library.
- Very early support of the standard C++ library.
- Supports DLDI, DSi SD slot and NitroFAT (open source alternative of NitroFS)
  through Elm's FatFs.
- Documentation on how to migrate projects to BlocksDS.
- Docker image provided.

