---
title: 'Changelog'
weight: -20
---

## Version 1.12.0 (2025-07-12)

- libnds:

  - Rename symbols of FatFs inside libnds so that users can have their own copy
    of FatFs. @asiekierka
  - Improve `normalizef32()`: Fix normalization of large and small vectors and
    optimize its performance. @Kuratius, @19tracks
  - Fix base address of the bootstub struct.
  - Clarify documentation about how `keysDownRepeat()` works.
  - Move some key state handling to critical sections to prevent race
    conditions.
  - Fix implementation of `swiIntrWait()` on both CPUs. It has been rewritten in
    C and moved to the common folder so that both CPUs use the same
    implementation.
  - Two defines have been added for `swiIntrWait()`: `INTRWAIT_KEEP_FLAGS` and
    `INTRWAIT_CLEAR_FLAGS`.
  - Improve FIFO communications code:

    - In FIFO wait loops, don't discard current interrupts flags when calling
      `swiIntrWait()`. If a FIFO interrupt has happened, exit the wait loop
      right away.
    - In `fifoWait*()`, check `REG_IME` before calling `swiIntrWait()`. If
      interrupts are disabled, skip the wait. `swiIntrWait()` enables interrupts
      internally, and this may be an issue for code that relies on interrupts
      being disabled. The only side-effect of not calling `swiIntrWait()` is
      that the CPU won't be able to enter low-power mode.
    - Fix some race conditions by moving some code and checks to critical
      sections.
    - Add some new definitions related to FIFO interrupts that are clearer than
      the previous ones.
    - Prevent deadlocks in some situations. For example, if the software RX
      queue was almost full, the hardware RX queue is half emptied in the IRQ
      handler, but it can't be completely emptied, the FIFO code would never
      check the hardware RX queue again. There were similar situations with the
      TX queue. The new code adds checks for the RX and TX hardware queues in
      some other places (like `fifoCheck*()` or `fifoInternalSend()`) that are
      likely to be executed in any potential user wait loop.
    - The FIFO buffer has been renamed to global FIFO pool. The send and receive
      queues have been renamed to RX and TX queues.
    - Fix global pool corruption when the pool got filled with packets that
      hadn't been handled. The code that allocates and frees blocks from the
      pool can't handle the situation in which zero blocks are left, so the new
      code makes sure that there is always at least one free block in the pool.
    - The code has been documented, and things like the global pool of blocks
      have been refactored to be much clearer.
    - Unknown system FIFO commands will now crash the application instead of
      being ignored.

  - Improve cothread system:

    - Show a crash message when trying to remove a nonexistent cothread context.
    - Optimize `cothread_yield_irq()`.
    - Prevent `cothread_yield()`, `cothread_yield_irq()` and
      `cothread_yield_irq_aux()` from inside  interrupt handlers. Instead of
      yielding, `cothread_yield()` returns right away, and the others call
      `swiIntrWait()` instead.
    - `swiIntrWait()` is now only called if `REG_IME=1`. If not, it is skipped.

  - Refactor global IRQ handler:

    - Remove the `irqDummy()` function, use `NULL` pointers instead.
    - Use fewer cycles to exit the handler when there isn't an user interrupt
      handler assigned to the interrupt.
    - Optimize code that wakes up threads waiting for an interrupt, preventing
      racen conditions.

  - Compile `getHeap*()` functions on the ARM7 too. @asiekierka

- Maxmod:

  - Fix Portamento + Volume Slide effect.

- Grit:

  - Crash if mode append is selected but it's not allowed due to other settings.
    In the past, grit would just overwrite the invalid setting without warning
    the user.
  - Add note to the documentation about how it is better to reduce quality of
    graphics before passing them to grit than to let grit reduce the quality.

- SDK:

  - Documentation:

    - Add a section to the "Usage notes" page about how to move the DTCM user
      variables to the end of DTCM using the `__dtcm_data_size` symbol.

  - Examples:

    - Fix example of streaming audio with Maxmod. Instead of reading the file
      from the Maxmod callback handler (which is inside an interrupt handler)
      the example now keeps a circular buffer. The main loop writes new data
      read from the file and the interrupt handler reads the data and sends it
      to Maxmod.
    - Add example of displaying a 4 BPP tiled background. Rename the previous
      `bg_regular` example to `bg_regular_8bit` to clarify that it loads a 8 BPP
      tiled background.
    - Add example of creating an FPS counter.
    - Add example of how to use the VBL interrupt.
    - Improve C++ example to also run on the ARM7.
    - Add example of how to use DMA to do HBLANK scroll effects.
    - Add example of how to save and load data from the filesystem.
    - Improve `key_input` example to show the values returned by
      `keysDownRepeat()`.
    - Fix build of the example that prints no$gba debug messages from both CPUs.
      Floating point support has been removed from `printf()` on the ARM7 to
      save space.

  - Tests:

    - The exit to loader test has been fixed (even though the exit code doesn't
      work properly yet).
    - Add a test to see how to exit CPU halt state with IRQs.
    - Add a test to see the effect of deleting threads in different ways.
    - Add a test to check that CPU contexts are preserved after returning from a
      cothread yield.
    - Add a test to check the behaviour of the FIFO handling code when the FIFO
      hardware and software queues are full.
    - Some test folders have been moved around for clarity.
    - Add test of using the `__dtcm_data_size` symbol.

  - Other:

    - Modify the default makefiles to allow the user to specify additional
      `LDFLAGS` when including them.

## Version 1.11.1 (2025-05-30)

- libnds:

  - Fix a bug that would crash the application if `__aeabi_atexit()` was called
    when a dynamic library isn't being loaded.

- SDK:

  - Fix linker scripts to include the picolibc hook that calls all functions
    registered with the `atexit()` family of functions. This also affected
    global destructors, which weren't called at exit. @asiekierka
  - Improve C++ example to test global destructors in addition to global
    constructors.

## Version 1.11.0 (2025-05-29)

- libnds:

  - Improve performance of camera driver significantly.
  - Support using TLS symbols from the main binary in dynamic libraries.
  - Support `R_ARM_JUMP24` relocation in dynamic libraries, which tends to be
    used for tail function calls in ARM mode.
  - Support global constructors and destructors in dynamic libraries. They are
    called when libraries are loaded and unloaded.
  - Add `dlmembase()` function to get the base address of a loaded dynamic
    library, which can be used to load the ELF file at the right address when
    debugging the program with GDB.
  - Add defines to manually place variables in ITCM.
  - The two variables placed in DTCM by the cothreads code have been moved to
    ITCM to help the stack grow (they were being placed at the start of DTCM,
    which stopped the stack from growing into main RAM).
  - Fix `grfLoadMemEx()` and `grfLoadFileEx()` when passing `NULL` in `header`.
  - A few improvements to `hw_sqrtf()`. @Kuratius
  - Minor documentation fixes. @Kuratius
  - Reintroduce list of copyright holders to the license file. @asiekierka
  - Uniformize license headers and update the list of copyright holders in the
    license file.

- Maxmod:

  - Fix some size definitions in the GBA public headers.
  - Fix memory leaks when unloading modules. @ds-sloth
  - A lot of assembly code has been converted to C, particularly code related to
    the module player and effects handling.
  - Fix bug when stopping the playback of a module. This caused noise to be
    generated in the GBA port when a song was stopped.
  - The DS programming guide has been fixed.

- mmutil:

  - Add readme to repository.

- SDK:

  - dsltool:

    - Support using TLS symbols from the main binary in dynamic libraries.
    - Support `R_ARM_JUMP24` relocations.
    - Support loading ELF files with more than 256 symbols. @jonko0493
    - Add link to documentation about relocations.
    - Support global constructors and destructors.
    - Provide `__dso_handle` in all dynamic libraries.

  - Examples:

    - Add C constructor to basic dynamic library example and improve it in
      general.
    - Add example of using C++ dynamic libraries.
    - Let user stop song in audio modes Maxmod example.

  - Other:

    - Document limitations of dynamic libraries and how to debug them.
    - Enable default exception handler in the default debug ARM7 cores.
    - Clarify hack used in the crt0 to reference some symbols and prevent the
      garbage collector of the linker from removing it.


## Version 1.10.1 (2025-04-26)

- SDK:

  - Other:

    - Global initializers have been fixed for picolibc 1.8.10.
    - Fix conflicting types when including `math.h` instead of `cmath` from C++
      files.

## Version 1.10.0 (2025-04-25)

- Maxmod:

  - Most of the files of the library have been converted from assembly to C to
    help people understand, maintain and improve the code. The remaining
    assembly code is the software mixing code of the ARM7 (on both DS and GBA
    modes) and the song player code (which is too big to convert in one go and
    it will require more work in the future). Both the GBA and DS ports have
    been tested. Thanks to @Lorenzooone for all his work!
  - Some error checks have been added to code that didn't have it (like all code
    using `fopen()`, `fread()` or `malloc()`.
  - Code that allocates memory now uses `calloc()` instead of `malloc()` to
    clear the buffer before using it.
  - The unused Value32 FIFO handler code in the ARM7 has been removed.
  - Cache management has been fixed. The cache handling functions of Maxmod were
    incorrect, and they have been removed. Now Maxmod uses the functions of
    libnds.
  - IPC code that synchronized streaming between ARM7 and ARM9 has been
    improved. Instead of using shared memory to synchronize CPUs, new FIFO
    messages have been implemented. The old code required dangerous cache
    management that could corrupt variables surounding the "ready" flag used by
    the library.

- SDK:

  - Other:

    - GCC has been updated to version 15.1.0 in Wonderful Toolchain. You can
      check the release notes [here](https://gcc.gnu.org/gcc-15/changes.html).
      An important addition is the support for `#embed`.
    - picolibc has been updated to version 1.8.10 in Wonderful Toolchain. Check
      the release notes [here](https://github.com/picolibc/picolibc/releases/tag/1.8.10).

  - Examples:

    - A new example of how to use sound effects with Maxmod has been added.
    - The Maxmod streaming example has been fixed to not hang in DSi consoles or
      DS consoles with DLDI running on the ARM7.
    - The reverb Maxmod example has bee improved with a new song that makes the
      effect more evident.
    - The "sprites" example has been renamed to "sprites_regular" for clarity.
      Also, 128 KB sprite mapping mode is now used in this example, as this is
      what most developers would want in their own code.
    - Some comments have been added to the libnds sound example.
    - A missing example description has been added.

## Version 1.9.1 (2025-04-03)

- libnds:

  - A new function has been added to reduce the heap space from the end of RAM:
    `reduceHeapSize()`.
  - The sprite mapping enums have been simplified by removing a part of the
    value that wasn't used by anything.
  - A few documentation improvements in backgrounds, sprites and FIFO
    definitions.

- SDK:

  - Documentation:

    - The memory map documentation has been updated.

  - Examples:

    - The images used in 2D sprites and backgrounds examples have been improved.
    - A new example has been added to show how to use main engine video mode 6.
    - A new example has been added to show how to use extended affine
      backgrounds. There was only an example of how to use regular affine
      backgrounds.
    - The example of sprites that use extended palettes has been updated to use
      multiple palettes per engine instead of just one.

  - Tests:

    - A new test has been added to check the bounds of the heap compared to the
      bounds of DTCM. It also checks if `reduceHeapSize()`, `malloc()` and
      `free()` interact the right way.

  - crts:

    - A very old bug has been fixed where the heap end in DSi mode would allow
      malloc() to allocate memory inside DTCM (which is used by the stack, and
      would corrupt it). Thanks to @ds-sloth for the initial report that
      resulted in finding the bug.

  - Other:

    - A new tutorial for BlocksDS has been started. It's available
      [here](https://blocksds.skylyrac.net/tutorial/), and the source code is
      available [here](https://github.com/blocksds/tutorial).

## Version 1.9.0 (2025-03-26)

- libnds:

  - Add initial helpers to load dynamic libraries with `dlfcn.h` functions. Note
    that this is still experimental. `dlopen()`, `dlsym()`, `dlerror()` and
    `dlclose()` are supported.
  - Fixed global C++ constructors that require libnds and the cothread scheduler
    to be initialized. Some functions previously called from the crt0 have been
    moved to `cothread_main()`, which runs after there is a valid multithreading
    environment.
  - Deallocate memory reserved for the FAT file system cache if `fatInit()`
    fails.
  - Extend `statvfs()` to return the `ST_RDONLY` flag for read-only media.
  - File system operations should now return `EROFS` for read-only DLDI
    drivers.
  - Made `libndsCrash()` public.
  - Fix potential issues involving FAT file system cache initialization.
  - Add some checks to libc system call functions like `open()`.

- DSWifi:

  - Fix `ioctl(FIONREAD, x)`, which used to return `EINVAL` even when it
    succeeded.
  - Remove Access Points from the list of availabled APs after enough time has
    passed without receiving any beacon packet from them. `Wifi_GetData()` and
    `Wifi_GetAPData()` have been updated to work with the changed system.
  - The array that specifies the order in which channels are scanned has been
    modified so that each channel is scanned the same number of times. This
    makes it easier to determine which APs to remove from the list of APs
    because we aren't prioritizing APs from any channel. Also, scan mode now
    always start with channel 1.
  - Old RSSI values of APs are now discarded instead of being used to average
    the value of the RSSI. This doesn't really affect how the values behave in
    real life that much, so this change saves memory with no noticeable change
    in the behaviour of the library.
  - Return error on timeouts when writing to baseband chip.
  - Add error checks when initializing RF chip.
  - Print debug message if the ARM7 TX queue is full when the library tries to
    add a new packet to it.
  - Some documentation improvements.

- dsltool:

  - Introduced tool to the repository. This tool is used to convert dynamic
    libraries in ELF format to DSL format, which is similar to ELF, but
    simplified. This DSL format can be loaded by libnds with `dlopen()`.

- teaktool:

  - Ensure that ELF files are loaded correctly instead of crashing if they
    aren't loaded and the pointer is NULL.
  - Fix format in a printf.

- SDK:

  - Documentation:

    - The new dynamic library system has been documented in the main
      documentation of BlocksDS.

  - Examples:

    - Three new examples have been added to show how to use the new dynamic
      library system.

  - crts:

    - Some initialization has been moved away from the crt0 of the ARM9 to
      libnds to fix global C++ constructors that require libnds and the cothread
      scheduler to be initialized.
    - A new linkerscript to build dynamic libraries has been added.

  - Other:

    - Libraries are now built with debug symbols (`-g`) to help debug
      applications made with them.

## Version 1.8.1 (2025-03-07)

- libnds:

  - Add a helper to convert UTF-16LE text (like the firmware player name and
    message) to UTF-8.
  - Modify `assert()` on the ARM7 to send the information to the ARM9 instead of
    displaying it on the no$gba console.
  - Added `swiIntrWaitAUX()` for the ARM7.
  - The Doxygen documentation of interrupt functions has been fixed.
  - The documentation of the firmware personal data has been improved with a
    warning saying that the struct may not be initialized right af the beginning
    of `main()` on the ARM9.
  - Always use `fake_heap_start` and `fake_heap_end` in `sbrk()`.
  - Some assertions in the timer functions were missing checks for values lower
    than zero, this has been fixed.
  - All instances of `sassert()` in common ARM7 and ARM9 code have been replaced
    by `assert()`.

- DSWifi:

  - Add player name information to beacon packets, and add it to the
    `Wifi_AccessPoint` struct so that clients can see it. This name can be
    replaced by any string defined by the developer if required.
  - Add way to get the RSSI of the AP we're connected to. It can be done by
    calling `Wifi_GetData(WIFIGETDATA_RSSI)`. @Snowshoe
  - Always include `ASSOCSTATUS_STRINGS` array in builds. It used to be removed
    in builds without sgIP.
  - Some minor documentation fixes.

- grit:

  - Update documentation (command line interface and changelog).
  - Let the build system define the version string compiled in the binary.

- SDK:

  - Examples:

    - In DSWifi examples, print multiplayer access points in color red when they
      aren't accepting new connections. Also, display the player name provided
      by the beacon frames.
    - There's a new test to verify that `sbrk()` works correctly in both the
      ARM7 and ARM9.
    - Update the asserts example to show that asserts that happen in the ARM7
      are sent to the ARM9 and displayed on the screen.

  - Documentation:

    - Document flag used for "twl" sections in ndstool and the linkerscripts.

  - Other:

    - Remove duplicated `__end__` entries in ARM7 linkerscript files.
    - The fake heap pointers are now setup on the ARM7, not only the ARM9.
    - The fake heap limit setup has been documented.
    - The R4RF DLDI driver included in the SDK has been updated.
    - Update CMake build system to link the debug version of libnds in debug
      builds.

## Version 1.8.0 (2025-02-28)

- libnds:

  - videoGL now tracks the number of textures using a palettes correctly.
    Previously, palettes would be deleted even when there were textures using
    them. @sillysagiri
  - `scanf()` has been fixed so that it doesn't record modifier keys on the
    output string.
  - Fix `readFirmware()` and `writeFirmware()` on the ARM9. They would silently
    fail when the source or destination buffer wasn't in main RAM (for example,
    when the buffer was in the stack in DTCM) because the ARM7 can't access
    other memory regions. Now, the functions will allocate temporary buffers to
    transfer data between the ARM7 and ARM9. Also, now they use a mutex so that
    they are thread-safe.
  - FatFs has been updated to version R0.15ap1.
  - The values used by `ledBlink()` have been documented as an enum.
  - There's a new define for the WiFi hardware in `REG_POWERCNT`.
  - There are new definitions for GRF files to specify background types.
  - `consoleVprintf()` is now exposed on the ARM7.
  - In the GRF struct there used to be a byte of padding in the struct that
    wasn't documented. It has now been explicitly documented as padding.
  - A helper has been added to check if a buffer is inside of main RAM.
  - Some unused internal FIFO command names have been removed.

- DSWifi:

  - Local multiplayer support has been added (NiFi). One DS acts as a
    multiplayer host (Access Point) and the other consoles connect to it.
  - The old operation mode has been renamed "Internet mode". It is possible to
    switch between Internet and local multiplayer modes depending on the needs
    of the application.
  - Scan mode now works differently. In Internet mode it shows all Access
    Points. In local multiplayer mode it only lists multiplayer hosts, in
    Internet mode the behaviour remains unchanged.
  - CMD/REPLY packet transmit support has been added to the library. This is a
    way to transfer data between a host and many clients more efficiently.
    However, regular data packets can still be sent between multiplayer clients
    and the host if the CMD/REPLY system isn't adequate for the application.
  - Beacon packet transmission is now properly supported and documented. The
    packets advertise the number of players currently connected to a host DS and
    whether the host accepts new connections or not. It also shows the game ID
    defined by the user, which is used to identify the right AP for a game.
  - When connecting to an Access Point, DSWifi is now honest about the transfer
    rates supported by the DS. If the AP rejects the connection, it will retry
    with all the rates defined in the IEEE 802.11b standard (even though only
    the first 2 out of the 4 are supported), which helps the AP accept the DS.
  - Many new multiplayer-related functions have been added to the library.
  - New documentation has been added, which explains how the library works
    internally, and how to use DSWifi in Internet and local multiplayer mode.
  - Huge refactor of ARM7 code and the corresponding ARM9 code that communicates
    with the ARM7. sgIP has been largely untouched. The code has been split into
    several files, and there is now a clearer split between sgIP and the rest of
    DSWifi.
  - The helper functions to read the firmware and to set the LED blink status
    have been removed. Now DSWifi calls the libnds functions instead.
  - Functions have been documented, IEEE 802.11 frame formats have been
    documented.
  - There are new defines for hardware registers based on GBATEK, and new
    defines for the fields in the hardware registers that are in used. The
    [melonDS forums](https://melonds.kuribo64.net/board/thread.php?id=34) have a
    lot of information (by @Arisotura) that has also been used as source when
    documenting register fields (like `W_RXFILTER` and `W_RXFILTER2`).
  - Debug messages have been added to the ARM7 code of the library. They are
    only available in debug builds.
  - WEP password usage has been documented. It wasn't clear how to use Hex/ASCII
    passwords.
  - Some unused fields of the `WifiData` struct have been removed.
  - Private definitions have been removed from public headers.

- grit:

  - Update GRF export code to use the new background type definitions proposed
    by @Garhoogin in [ptexconv](https://github.com/Garhoogin/ptexconv/issues/3).

- SDK

  - Examples:

    - New DSWifi examples have been added, including local multiplayer examples
      that show how to send CMD/REPLY packets and regular data frames of
      arbitrary sizes.
    - The WEP password input in the AP list example has been improved. The
      output of the example has also been improved.
    - The "get website" example now uses socket-related functions the way DSWifi
      expects them (`closesocket()` instead of `close()`).
    - The templates and examples now use the new LED defines.

  - Documentation:

    - The documentation of DSWifi now explains how to use the Internet and local
      multiplayer modes, as well as how the library works internally.
    - The licensing requirements of some libraries (picolibc, libstdc++) has
      been clarified by @asiekierka.

  - Other:

    - New debug ARM7 cores have been added. They use the debug versions of
      libnds and DSWifi.
    - The CMake build system now uses a linker group for the standard libraries
      to prevent linking errors due to circular dependencies. Also, banner title
      formation has been improved.

## Version 1.7.3 (2025-01-15)

- libnds:

  - Use custom version of `swiSoftReset()` instead of instruction `swi #0` to
    prevent crashes on DSi mode, where `swi #0` isn't implemented.
  - `swi*()` functions that don't actually use `swi` instructions (because of
    bugs in the implementation in the BIOS) have been documented.
  - `nitroFSInit()` now sets `errno` correctly if there is no NitroFS filesystem
    in the ROM.
  - The DSi mode checks of `soundExtSetFrequencyTWL()` have been moved out of
    `twl` sections so that this function can be safely called even in DS mode
    (even if it doesn't have any effect).
  - Calling `irqSet(flags, NULL)` or `irqSetAUX(flags, NULL)` is now safe.
    Previously, this would cause `NULL` to be used as interrupt handler instead
    of disabling the interrupt handler as expected.
  - The type `IntFn` has been deprecated in favour of `VoidFn`. Even the
    `interrupts.h` header didn't use `IntFn`, it was only used internally in
    `interrupts.c`.

- SDK:

  - Docker:

    - The Docker images are now built in a GitHub Actions pipeline in the SDK
      repository instead of locally in the computers of the developers.

  - Examples:

    - In the LibXM7 example that uses NitroFS, set pointer to `NULL` when it
      stops being used so that the program keeps track of whether songs are
      playing or not. This fixes a crash when trying to start a song after
      stopping a song.

  - Documentation:

    - Document the release process for new BlocksDS versions.
    - Document the existence minimal ARM7 core.
    - Remove references to scripts related to the release process used before
      using GitHub Actions.
    - Add link to `ndslib-archive` repository so that it can be used as
      reference to understand old NDS homebrew source code.
    - Clarify how to build examples in the setup instructions.

  - Other:

    - Use the right system header in `dldipatch` to fix a compiler warning.
    - PAlib is now available as a pacman package: `blocksds-palib`

## Version 1.7.2 (2025-01-07)

- libnds:

  - Fix compilation error due to incorrect names of slot-2 rumble functions.
  - Add debug assertions to check that `cothread_yield_irq()` is never called
    with interrupts disabled (when `IME` is 0).
  - Small optimization in `cothread_yield_irq()` wait loop on the ARM9.
  - Add helper constants for `swiUnpackBits()`.
  - Implement `getBatteryLevel()` on the ARM7 to mirror the ARM9 implementation.
  - Add function to check if FatFs cache has been initialized or not.
  - Fix code to read official cartridges from the ARM7. The previous code wasn't
    able to read data when the destination was in the stack (DTCM isn't visible
    by the ARM7).
  - Add error checks on the ARM7 to crash if DLDI is used before being setup.
  - Fix undefined C behaviour with some left shifts.
  - Add missing system include to ARM7 console header.

- SDK:

  - ARM7:

    - Restructure default ARM7 code to allow for multiple built-in ARM7 cores.
      Add `ARM7ELF` to the default makefiles to easily select the ARM7 core.
      Thanks, @GalaxyShard.

  - Documentation:

    - Document the new pre-built ARM7 cores.

  - Examples:

    - Use the new pre-built ARM7 core in LibXM7 examples instead of custom ARM7
      cores.
    - Update the BIOS bit unpack example to use the new constants introduced to
      libnds.

  - Other:

    - In the makefiles of DSWiFi, LibXM7 and Maxmod use `ar` from the ARM
      toolchain rather than the host to build the library.
    - Improve scripts to release new SDK versions.

## Version 1.7.1 (2024-12-25)

- libnds:

  - Fixed a bug where some assembly functions would be removed by the linker.
    This happened when a function is supposed to fall through the following one,
    but it is in a different section. If this other section isn't referenced
    anywhere, the linker would remove it.
  - A new system to send formatted text messages from the ARM7 to the ARM9 has
    been implemented. This works by using a ring buffer in main RAM that is
    allocated from the ARM9, so the user can adjust its size easily. This is
    still being tested and it may change.

- SDK:

  - Docker:

    - Install BlocksDS from pacman packages instead of building it from source.
      This will make it easier to install other third-party packages. It won't
      be needed to build them from source and install them, you will be able to
      simply install the package. The slim image only has the
      `blocksds-toolchain` package, the dev image has all the `blocksds-*`
      packages.

  - Documentation:

    - Update setup instructions.

  - Examples:

    - New example of how to use the ARM7 console to print text on the screen.

## Version 1.7.0 (2024-12-23)

- libnds:

  - An exception handler has been added to the ARM7. This one is a lot less
    useful than the ARM9 one because it's only called on undefined instructions
    and internal libnds errors.
  - Add `dldiRelocate()`, which allows relocating a DLDI driver to a different
    memory address.
  - Fix DLDI memory pointers not being relocated for non-BSS/noinit areas.
  - Update FatFs to R0.15a.
  - `glColorSubTableEXT()` now supports allocating empty palettes by passing a
    NULL pointer in the `table` argument. This is a small compatibility break
    because this used to free the palette. However, the old behaviour stays as
    long as `width` is 0. In most cases, `table` would be NULL and `width` would
    be 0, so this change is only a problem in unusual cases.
  - Disable MPU safely before calling the exit-to-loader ARM9 hook in case the
    hook doesn't do it properly.

- maxmod:

  - Remove non-standard `.struct` directives.

- SDK:

  - picolibc:

    - Add `asnprintf()` and `vasnprintf()`, matching newlib.
    - Fix memory leak in `asprintf()` and `vasprintf()`.

  - ndstool:

    - Add a default icon for new `.nds` files, preventing the generation of invalid
      banners.
    - Add support for extracting static `.nds` icons as `.bmp` files.
    - Allow specifying `-bt` without a language ID.
    - Fix the `-n` (latency) argument being a no-op, add corresponding `-n1` argument.
    - Fix the `-fh` (fix header CRC) command not working with TWL-enhanced `.nds` files.

  - Docker:

    - curl has been added to both the development and slim images.
    - Add new BlocksDS pacman repository during setup.

  - pacman:

    - The pacman server of BlocksDS is now independent from Wonderful Toolchain.

      - The repository with the package build files is now in the BlocksDS
        GitHub organization in this URL: https://github.com/blocksds/packages
      - BlocksDS still uses Wonderful Toolchain as source for the compiler and
        the standard C and C++ libraries. It will keep using wf-pacman for the
        forseeable future as well.
      - Special thanks to asie for all his help.
      - Updating old installations requires the user to run the following
        commands:

        ```bash
        wf-pacman -Sy wf-pacman
        wf-pacman -Syu
        wf-pacman -Syu
        ```

    - New libraries are now available as pacman packages (µLibrary, libwoopsi,
      dsmi) as well as the tool ptexconv.
    - The GitHub pipeline of the SDK repository has been removed and added to
      the packages repository.

  - Documentation:

    - Update documentation with the instructions to use the new pacman
      repository.
    - Some outdated licensing information has been updated.
    - The docs generation script has been split into a script that generates it
      and a script that pushes the built documentation to the repository.
    - The updating guide has been updated for versions between 1.3.0 and 1.7.0.

  - Examples:

    - New example of gesture recognition using the $N Multistroke Recognizer.
    - New example of how to use CMake to build a project with BlocksDS.
    - Update the exception handler example to also support triggering ARM7
      exceptions.

  - Other:

    - BlocksDS now has a dedicated URL (https://blocksds.skylyrac.net/) which
      means that it doesn't rely on GitHub URLs anymore. This main page
      redirects to the documentation, GitHub organization, and a NDS homebrew
      development manual.
    - A new prototype CMake build system has been added to the repository.
    - `CC` makefile variables have been renamed to `HOSTCC` when they refer to
      building host tools. This will make it possible to overwrite the host
      toolchain without also overwriting the ARM toolchain.
    - Use standard C types and fix includes in mkfatimg.
    - bin2c now has an option (`--noext`) that lets it export files excluding
      the extension from the name, the way some very old homebrew projects
      expect it.
    - Some sample defines have been added to the makefiles of the templates to
      clarify its syntax.

## Version 1.6.3 (2024-11-11)

- libnds:

  - Add compile-time warnings about unused result values to various library
    methods.
  - Ensure `nitroFSInit()` checks for successful FAT initialization.
  - Protect the alternate vector base region (`0x0000000` onwards)
    from writing and, if said base is not explicitly enabled, reading.
    This allows catching null pointer accesses at runtime as data aborts.

- SDK:

  - crt0:

    - Fix thread-local storage not being initialized properly during
      global constructor initialization.
    - Fix thread-local storage not being initialized at all on ARM7.

  - picolibc:

    - Add `memset_explicit()`.
    - Improve wctype compatibility.
    - Update ctype case conversion table to Unicode 15.1.0.

  - Toolchain:

    - Update binutils to 2.43.1.
    - The alternate `gold` linker is no longer shipped with the toolchain.

  - Other:

    - Add regression test for `setVectorBase()`.
    - Modify templates to use toolchain `gcc-ar` over host `ar` for
      packaging libraries; this ensures correct operation on systems
      without `binutils` installed, as well as when building libraries
      with link-time optimized objects.
    - Update ARM assembly code to always use UAL syntax.
    - Update official Docker image to Ubuntu 24.04.

## Version 1.6.2 (2024-11-04)

- libnds:

  - Filesystem:

    - Fixed a bug involving incorrect reads of recently written sectors.

- SDK:

  - Changes to examples:

    - Clean the 8-bit double-buffered bitmap example.

  - Other:

    - Improve FAQ and updating guide.

## Version 1.6.1 (2024-11-03)

- libnds:

  - Filesystem:

    - This version was supposed to include a fix for a bug involving incorrect
      reads of recently written sectors, but the submodule wasn't updated
      correctly so the fix was left out.

## Version 1.6.0 (2024-11-03)

- libnds:

  - Peripherals:

    - Add support for the "retail" (ATTiny-based) Motion Pack accelerometer
      to the DS Motion Card/Pak driver.
    - Clean up the DS Motion Card/Pak driver.
    - Fix the DS Motion Card driver not working on DSi consoles.
    - Fix the solar sensor not being detected on Boktai 1 cartridges.
    - Fix `peripheralSlot2SolarScanFast()` not working correctly.

  - GL2D:

    - Fix off-by-one errors when drawing textures with GL2D when the textures
      aren't flipped.
    - Reorganize GL2D setup code to reduce code duplication.

  - Other:

    - Fix return types and improve performance of integer square root helpers.
    - In `initSystem()`, fix video register clearing ranges and remove redundant
      VRAM configuration.
    - The GRF "HDR" RIFF chunk has been deprecated and it has been replaced by
      a new chunk called "HDRX", which is exactly the same, but it includes a
      version field so that it is future-proof.
    - Fix some issues that prevented LLVM from building the library. Add missing
      arguments to UDF instructions in the codebase. Add some explicit casts. Use
      `inttypes.h` definitions in `printf()` instead of explicit size modifiers.
    - Fix undefined behaviour warnings due to left shifts of signed integers.

- grit:

  - Update GRF export code to match the new format used by libnds.

- ndstool:

  - Use `git describe` output as version string instead of a hardcoded outdated
    version number.

- SDK:

  - crt0 and linker script files:

    - It is now possible to place DTCM variables and data at the end of DTCM
      instead of the beginning. This means the stack can grow downwards without
      affecting the variables at the beginning of DTCM. This is done by setting
      the value of the new symbol `__dtcm_data_size` to a non-zero value. The
      linker will helpfully point out if the data area is too small for a given
      program.
    - The user-configurable symbol `__shared_wram_size` has been added.
    - Assertions have been added to check that the user-defined sizes produce
      a valid executable.
    - Support for `.noinit` sections has been added.
    - Missing copyright notices have been added to linker scripts.
    - Minor cleanups and adjustments have been done to the crt0 code.

  - picolibc:

    - Add implementation of `<uchar.h>`.
    - Improve `powf()` accuracy.

  - Documentation:

     - Provide information about the build process of .nds files.
     - Add a FAQ to the documentation.
     - Add IRC channels to the support channels page.
     - Add notes about what to do after installing BlocksDS.
     - Credit all known contributors to all repositories of BlocksDS.

  - Examples:

    - Add NDS Motion card example.
    - Add example of scaling and rotating GL2D sprites.
    - Add `inttypes.h` include in some examples that require it after picolibc
      changes.

## Version 1.5.0 (2024-09-10)

- libnds:

  - Exception handling:

    - Add trivial crash handler to inform the user of unrecoverable error
      conditions, such as stack corruptions with stack smash protection
      enabled.
    - Add trivial release exception handler to inform the user of data aborts
      and other exception conditions even in release builds. This is enabled
      by default; this implementation omits stack and register dumps, making
      the `.nds` size increase negligible.
    - Tweak default exception handler to use a darker red for the background.

  - Filesystem:

    - Fix using Slot-2 direct access on `.nds` files over 32 MB. In particular,
      this fixes accessing large NitroFS filesystems under NO$GBA.

  - GRF loader:

    - Improve performance by using fast `memcpy()` over `swiCopy()`.
    - Rework loader to only read requested chunks from the filesystem,
      as opposed to reading all chunks and filtering them at a later stage.

  - Sound:

    - Add functions to play PSG, noise and sound samples on specific channels.
    - Add `soundCaptureStart` and `soundCaptureStop` high-level helpers for
      the sound capture functionality.

  - Other:

    - Add additional error checks to NitroFS and GRF functions.
    - Add unused result warnings to functions which can cause memory leaks if
      their result is not deallocated.
    - Document high-level DSi camera functions.
    - Fix console 'K' ANSI escape code not clearing characters on the line.
    - Fix includes for the latest picolibc commits.

- SDK:

  - New examples:

    - Add ANSI escape code tests.
    - Add sound capture example.

  - Changes to examples:

    - Add `.MOD` files to LibXM7 examples.
    - Add missing `fclose()` error checks.
    - Adjust examples using GRF files to showcase the loader's built-in
      asset decompression functionality.
    - Clean up camera and LibXM7 examples.

  - picolibc:

    - Add support for C11 Annex K functions (`_s`-suffixed functions).
    - Deprecate BSD-specific `<sys/tree.h>`.
    - Fix handling `NaN` in `scanf()` and `strtod()`.
    - Fix return values for `glob()`.
    - Fix the `FD_ZERO()` macro.
    - Initial support for POSIX.1-2024 in header files.
    - Other minor header file cleanups.
    - Replace `<_ansi.h>` with `<sys/cdefs.h>`.

  - Other:

    - Fix support for placing the Maxmod soundbank file in the NitroFS
      file system.
    - Make the SD card root directory configurable in default Makefile
      templates.
    - Remove Teak LLVM from the `-slim` Docker image. Most people don't need
      this toolchain, as it is in a highly experimental and unmaintained
      state; in addition, removing it from the image does not prevent people
      from installing it on top, but significantly reduces the download size
      for everyone else.
    - Remove usage of `__DATE__` in teaktool.

## Version 1.4.0 (2024-08-08)

- libnds:

  - Console:

    - Fix 'D' ANSI escape code, which was broken in version 1.3.1.
    - Document PrintConsole struct fields. Some of them are only meant to be
      used by libnds, which has been documented.
    - The size of some fields in the console struct has been reduced, and the
      fields have been reordered to reduce padding in the struct. Some fields
      were unused, and they have been removed.
    - Now it's possible to specify the 16-color palette to be used when loading
      custom fonts with their own palette.
    - `fontCharOffset` now works, which means that it is possible to specify
      that a font should be loaded after another font, so that two fonts can use
      the same tile block and save VRAM.
    - In order to be able to specify the palette and the `fontCharOffset`, a new
      function has been created: `consoleInitEx()`.
    - Mark the defaultConsole struct as const. It is needed to keep that data
      untouched because the user may initialize the default console several
      times in a program, and it's important to be able to return to the right
      initial state.
    - Some variables have been made private.
    - When a character outside of the character set of the font is printed, a
      space is now printed instead. Previously, it printed tiles outside of the
      font, which could result in garbage being printed on the screen.
    - New functions have been added to control the console without using awkward
      ANSI escape sequences: `consoleSetCursor()`, `consoleGetCursor()`,
      `consoleAddToCursor()` and `consoleSetColor()`.

  - Keyboard:

    - Make custom keyboards work. This seems to have never worked because libnds
      overwrote the custom keyboard struct with the default keyboard struct.
    - Keyboard struct fields have been refactored. Some field sizes are now
      smaller, some fields are now const. The order has been changed to reduce
      padding between fields and save even more RAM.
    - Ensure that the keyboard is always initialized to the right state with
      `keyboardShow()`. This lets you make a keyboard start in uppercase or
      lowercase state depending on your preferences.
    - Prevent keyboard functions from hanging if no keyboard is actually
      visible.

  - Other:

    - Fix crash when calling `peripheralSlot2InitDefault()` in a DSi retail
      unit. This didn't happen in DS, or in debugger DSi units.
    - Add helper to tell the GPU to apply fog to the clear buffer.
    - Flush RAM before copying console graphics to VRAM.
    - Implement `assert()` in the ARM9 and ARM7. In the ARM9 it works like
      `sassert()`, in the ARM7 it prints a no$gba debug message.
    - Clarify some comments about decompression routines of the BIOS.
    - Stop using ANSI escape sequences everywhere. Use direct console API
      functions instead, which are more self-explanatory.
    - The functions `sysGetCartOwner()` and `sysGetCardOwner()` have been added
      to be able to check the CPU owner of the Slot-1 and Slot-2 buses.
    - Add documentation to some functions without it, and fix some Doxygen
      comments.

- DSWiFi:

  - Stop generating `dswifi_version.h` as part of the build process.
  - Some warnings have been fixed.

- LibXM7:

  - Fix a crash when unloading MOD files.

- ndstool:

  - Fix NitroFS filesystem corruption when no banner is generated.

- grit:

  - Fix `libplum` build error with LLVM.

- mmutil:

  - Fix some incorrect types.

- SDK:

  - New examples:

    - Using fog in a 3D scene.
    - Sorting 3D translucent objects based on their distance to the camera and
      drawing them in the correct order.
    - How to use Maxmod song events.
    - How to stream a WAV file from the filesystem using Maxmod.
    - How to use the sound API of libnds to play PSG, noise and PCM audio.
    - How to use custom keyboards with libnds.

  - Changes to examples:

    - The basic Maxmod example now also shows how to play sound effects.
    - The example that loads custom fonts with the libnds console has been
      improved to load 2 fonts with custom 16-color graphics on one screen and
      one font with 256-color graphics on the other one.
    - Examples and test now don't use ANSI escape sequences. This makes the code
      a lot harder to understand.

  - Other:

    - Add page to the documentation about how to debug programs with libnds.
    - dlditool: Use `stdbool.h` instead of a custom boolean type.
    - Doxygen comments have been reorganized in all repositories so that they
      are uniform across all the codebase.
    - All Doxyfiles have been updated.

## Version 1.3.1 (2024-07-26)

- libnds:

  - Added memory barriers to libnds functions which remap user-accessible
    memory.
  - `consolePrintChar()` has been exposed in the public API. Coupled with
    internal refactors, this allows using the built-in console without
    pulling in printf/scanf as a dependency.
  - Fixed function parameters and improved parameters for BIOS RSA functions.
  - Fixed potential memory corruption when `micStopRecording()` was called
    twice.
  - Fixed regression in new touchscreen driver on CDC (TWL).
  - Improved documentation for the BIOS CRC-16 calculation function.
  - Minor optimizations to videoGL and console code.

- SDK:

  - New examples:

    - Added a 3D billboard drawing example.
    - Added a 3D spot lights example.
    - Added a 3D two-pass rendering example.
    - Added a BIOS bit unpacking example.
    - Added a BIOS CRC-16 calculation example.
    - Added a BIOS RSA decryption example.
    - Added a microphone recording example.
    - Added a Maxmod audio modes example.
    - Added a Maxmod reverb example.
    - Fixed a regression in the exception handler example.

  - picolibc:

    - The bundled version of picolibc has been updated.
    - Added an implementation of `posix_memalign()`.
    - Added support for `%a` and `%A` formats in `scanf()`.
    - Fixed `freopen()` not resetting the unget buffer.
    - Fixed hexadecimal string conversion in `strtod()`, `strtof()` and
      `strtold()`.
    - Fixed `lgammal()` corrupting the application-defined `signgam`
      value.
    - Fixed potential issue when calling `fclose()` more than once.
    - Fixed `printf()` rounding issues for `%a` and `%f` formats.
    - General header cleanups have been performed.

  - Other:

    - Expanded the touch input test to allow sampling input on initial pen
      press only, as opposed to every frame while the pen is touching the
      display.

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
    issues, forks have been created under the BlocksDS organization. It is
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
