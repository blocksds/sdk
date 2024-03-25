---
title: 'libc port information'
weight: 0
---

## 1. Introduction

BlocksDS has code to allow libc functions to work on NDS programs. The supported
functions are described in this file. Note that they should only be used from
ARM9 code, as they aren't supported from the ARM7.

## 2. argc and argv

For NDS and NDS Lite consoles that run ROMs from a flashcard, the loader needs
to support the argv protocol of libnds. If so, `argv[0]` will hold the path of
the NDS file in the SD card of the flashcard. The drive name is `fat:`. For
example, a valid path is `fat:/path/to/rom.nds`.

For DSi consoles loading ROMs from the internal SD card, `argv[0]` works in a
similar way. It will hold the path to the NDS ROM in the internal SD card, which
uses `sd:` as prefix. For example, `sd:/path/to/rom.nds`.

Note that it is possible to use a NDS flashcard on DSi. If the user loads the
NDS ROM from a flashcard, `argv[0]` will point to the filesystem of the
flashcard (`fat:`), not the internal SD card (`sd:`).

Other `argv` entries may be set as well, but this isn't a common occurrence.

For more information, check [this](https://devkitpro.org/wiki/Homebrew_Menu).

## 3. Filesystem

Functions like `fopen`, `opendir`, etc. For more information, check the
[filesystem documentation](../filesystem).

## 4. NitroFS

NitroFS (`nitro:`) is supported, but the implementation is different than the
one in `libfilesystem`. It should be 100% compatible with it. Please, report
any bugs you may find with it.

## 5. Text console (stdout, stderr, stdin)

`stdout` and `stderr` can be redirected to the libnds console or to the no$gba
debug console. They are unbuffered. When text is sent to them (by using
`printf()` or `fprintf(stderr, ...)` it's always sent to the console.  The
exceptions are ANSI escape sequences, which are buffered until the end of the
sequence is received, and then sent to the console low level functions.

Remember that you can use `fflush(stdout)` or `fflush(stderr)` if buffering
becomes an issue.

For more information about ANSI escape sequences, check
[this link](https://en.wikipedia.org/wiki/ANSI_escape_code).

`stdin` is tied to the keyboard of libnds. When `sscanf(stdin, ...)` is called,
for example, the keyboard of libnds is used as input device. Please, check the
examples to see how to use it.

## 6. Memory allocation

Functions like `malloc`, `calloc`, `memalign` and `free` work as usual.  They
always return space from the main RAM region.

## 7. Time

`gettimeofday` and `time` are supported.

## 8. Exit

If the NDS ROM loader supports it, `exit` can be used to return to the loader.
You can also return from `main()` to get the same effect.

For more information, check [this document](../exit_to_loader).

## 9. Multithreading support

Functions like `fopen()` or `malloc()` need to be thread-safe, and picolibc uses
locks to ensure that they are thread-safe. The implementation of the locking
funtrions is platform-specific, and libnds has its implementation in
[this file](https://github.com/blocksds/libnds/blob/8e7ab8207dfb761cd9984719690ac0c318f2f1ca/source/common/libc/locks.c).

In general, you don't need to worry about them. However, if there is an
unexpected crash because of running the undefined instruction `0xEBAD`, it may
mean that the locking functions have found a logic error.

Printing an error message is very expensive: It is required to include a text
font as well as console handling functions. The locking functions simply have
a [function](https://github.com/blocksds/libnds/blob/8e7ab8207dfb761cd9984719690ac0c318f2f1ca/source/common/libc/locks.c#L34)
with an opcode that is defined to always be an undefined instruction in Thumb
mode by the ARM Architecture Reference Manual.
