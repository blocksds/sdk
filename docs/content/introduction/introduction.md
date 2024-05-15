---
title: 'Introduction'
weight: -50
---

This is an SDK to develop applications for Nintendo DS in C or C++.

## 1. Goals

- To create an easy-to-use NDS software development kit based on GCC with as few
  dependencies as possible, and that supports as many features of the DS and DSi
  as possible.

- To encourage developers to hack and improve the SDK by letting them have all
  the source code of the SDK. This also shows how easy it is to build the whole
  codebase. The exception is the toolchain, which is non-trivial to build.
  Because of that, [Wonderful Toolchains](https://wonderful.asie.pl/) is used
  instead of asking the users to build it themselves.

- To try to keep compatibility with pre-existing C projects made by devkitARM.
  In theory, it should be enough to replace the Makefile of devkitARM by a
  Makefile of BlocksDS to build any old project, with some few exceptions. Check
  [this guide](../../technical/devkitarm_porting_guide) for more details.

- To document as much of the SDK as possible. For detailed information about the
  design of BlocksDS, check [this document](../../technical/design_guide).

- To ensure that the licensing status of all the code is correct and that the
  licenses used are all FOSS. This SDK is made from the work of many others that
  came before it and who developed FOSS libraries that can be now used by
  BlocksDS. This SDK must ensure that all the code remains FOSS for other people
  to use it in the future. For more information about the licenses used by
  different components, check [this](../licenses).

## 2. Features

### 2.1 Standard libraries

- Binaries are pretty small and their baseline RAM usage is low thanks to using
  [picolibc](https://github.com/picolibc/picolibc) instead of the more commonly
  used [newlib](https://sourceware.org/newlib/).

- A libc with most of the features and functions you would expect in a regular
  environment like in a PC. For example: `printf()`, `scanf()`, `fopen()`,
  `stat()`, `opendir()`, `malloc()`, `gettimeofday()`, `time()` and `exit()`.

  There are versions of functions like `memcpy()`, `memset()` and `memmove()`
  that have been optimized for the CPUs of the DS. This is thanks to using
  **ndsabi**, a fork of [agbabi](https://github.com/felixjones/agbabi) by
  felixjones.

  Also, `argv[]` is may be provided by the environment so that the program
  knows where it's running from, and any arguments, if any, passed to it.

- It is possible to access the filesystem of DLDI devices (like the SD card of
  flashcards), the SD slot of the DSi, and the filesystem embedded in a NDS ROM
  (NitroFS). Performance (particularly for reads) has been
  [optimized by asie](https://mk.asie.pl/notes/9m2fsvk1hp) a lot. This is partly
  thanks to using [FatFs](http://elm-chan.org/fsw/ff/00index_e.html), partly due
  to the DSi SD driver by [profi200](https://github.com/profi200/), and partly
  due to lots of additional optimizations by [asie](https://asie.pl/)

- C++ is supported, as well as `libstdc++` but it hasn't been tested as
  extensively as the C standard library.

- There is basic cooperative multithreading support. ``libnds`` has been
  modified to be thread-safe. Thread-local storage support has also been added.
  You can define variables that are unique per thread like this:

  ```c
  __thread int my_variable = 1000;
  __thread int other_variable;
  ```

  Filesystem reads can be done from the ARM9 and ARM7. You can make the ARM9 ask
  the ARM7 to load files in one thread. While the ARM7 load files the ARM9 can
  be doing other things in other threads, so you aren't blocked. This is always
  done when loading files from the SD card of the DSi, but it can also be done
  with DLDI devices. Most DLDI drivers can run from the ARM7 even if they
  haven't been explicitly built with that in mind!

### 2.2 Supported DS features

- All features of the 2D video hardware are supported. There are some
  APIs to simplify the usage of backgrounds and sprites, as well as a basic text
  console system based on the 2D background system.

- All features of the 3D hardware are supported through an API that is similar
  to the one of OpenGL 1. There is a library to render 2D graphics with the 3D
  hardware easily, called GL2D.

- There is a tool to convert graphics to the formats used by the DS: `grit`. The
  only format that it doesn't support is the `Tex4x4` texture format, but that
  one is supported by [**ptexconv**](https://github.com/Garhoogin/ptexconv/).

- The audio hardware is supported. There are two libraries that can be used to
  play music: Maxmod and libxm7. libxm7 only supports playing MOD and XM files,
  and it is required to use libnds to play SFXs through a basic sound API.
  Maxmod, on the other hand, has its own way to play sound effects and music,
  and it supports MOD, XM, IT and S3M files.

- It is possible to record audio using the microphone of the console.

- There are some helpers to use the division and square root co-processor, the
  BIOS functions, hardware timers, DMA, interrupt handlers, and the hardware
  FIFO system for ARM9 <-> ARM7 communications.

- There are some helpers to setup the MPU and to manipulate the cache of the
  ARM9 (to clean it or invalidate it).

- There is an API to access the DS cartridge ROM, as well as EEPROM chips in
  the cartridges that use it. It is also possible to access the filesystem of SD
  cards used in flashcards.

- There is an API to read the state of the buttons and the touchscreen.

- There is an API to use Slot-2 peripherals such as RAM expansion cartridges,
  gyroscopes or tilt sensors.

- There are some helper functions to decompress some formats supported by the
  BIOS.

- It is possible to read and set the date and time of the RTC clock.

- WiFi support isn't complete. It is possible to connect to Access Points with
  the library DS WiFi, but it doesn't support DS to DS WiFi communications.

- **ndstool** suuports creating NDS ROMs and define a ROM icon and banner text
  (in multiple languages!).

### 2.3 Supported DSi features

- The SD slot of the DSi is supported.

- The new DMA features, as well as the new WRAM memory banks, are supported.

- **ndstool** supports animated icons from PNG, GIF, even animated GIF files.

- The DSi camera is supported (thanks, asie!). It's still quite basic, but
  enough to be able to take photos from the front and back cameras.

- There is preliminary support for the DSP of the DSi (the toolchain is very
  buggy, so be very careful when using it, it's probably not ready to be used by
  most developers!).

- Unfortunately, none of the DSi WiFi features (such as WPA support) are
  supported by DS WiFi yet.
