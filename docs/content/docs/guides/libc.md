---
title: 'C/C++ support'
weight: 6
---

### 1. Introduction

BlocksDS supports modern C/C++ standard libraries by providing the following
libraries:

* [picolibc](https://github.com/picolibc/picolibc), an embedded-centric fork of
  the Newlib C library;
* [libstdc++](https://gcc.gnu.org/onlinedocs/libstdc++/), the GNU C++ library.

However, this support includes some platform-specific caveats.

### 2. POSIX I/O

BlocksDS provides POSIX standard I/O using the tinystdio library, which is part
of picolibc. It supports most of the functionality mandated by the C standard,
as well as some extensions.

File I/O (`fopen`, `opendir`, etc.) is supported. Storage on cartridges and the
DSi internal SD card can be accessed. A bundled read-only filesystem called
NitroFS is also supported. For more information, check the [filesystem documentation](../filesystem).

Console I/O (`stdin`, `stdout`) is supported; however, by default, no input or
output is initialized:

* `stdout` can be redirected to the built-in libnds console by initializing the
  console. This can be done using the `consoleInit()` or `consoleDemoInit()`
  functions.
* `stderr` can be redirected to the built-in libnds console or
  a no$gba-compatible emulator debug output by using `consoleDebugInit()`.

Standard output is unbuffered. When text is sent to them (by using `printf()`
or `fprintf(stderr, ...)` it's always sent to the console.  The exceptions
are ANSI escape sequences, which are buffered until the end of the sequence
is received, and then sent to the console low level functions.

Remember that you can use `fflush(stdout)` or `fflush(stderr)` if buffering
becomes an issue.

For more information about ANSI escape sequences, check
[this link](https://en.wikipedia.org/wiki/ANSI_escape_code).

* `stdin` is tied to the keyboard of libnds. When `sscanf(stdin, ...)` is called,
for example, the keyboard of libnds is used as input device. For more information,
see the relevant examples bundled with BlocksDS.

### 3. argc and argv

To provide correct argv information, the loader needs to support argv structure
passing. This is not supported by certain legacy loaders; for more information,
see our page on [legacy support](../../guides/legacy_support).

Under compatible loading environment, `argv[0]` will hold the path of the .nds
file. Other `argv` entries may be set as well, but this isn't a common
occurrence.

* For cartridges, the drive name used is `fat:` - for example, `fat:/homebrew/program.nds`.
* For DSi consoles loading .nds files from the internal SD card, the drive
  name used is `sd:` - for example, `sd:/homebrew/program.nds`.

### 4. Dynamic heap allocation

Functions like `malloc`, `calloc`, `memalign` and `free` are supported. They
always return space from the main RAM region.

### 5. Time

`gettimeofday` and `time` are supported. Timezones are not handled in any
particular manner, however.

### 6. Exit

If the NDS ROM loader supports it, `exit` can be used to return to the loader.
You can also return from `main()` to get the same effect.

For more information, check [this document](../../internal/exit_to_loader).

In addition, `atexit` is supported for dynamically registering program exit
handlers.

### 7. Multithreading

Functions like `fopen()` or `malloc()` need to be thread-safe, and picolibc uses
locks to ensure that they are thread-safe. The implementation of the locking
funtrions is platform-specific, and libnds has its implementation in
[this file](https://codeberg.org/blocksds/libnds/src/commit/25889452ac651d2e66dc652c5210cbc8b8199fc2/source/common/libc/locks.c).

In general, you don't need to worry about them. However, it's possible for them
to fail under very unusual situations and that will trigger a crash that shows
an error message such as "Lock init".
