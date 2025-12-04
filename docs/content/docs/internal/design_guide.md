---
title: 'Design choices'
weight: 1
---

This guide serves as a loose explanation of some design choices made during
the development of BlocksDS.

A supplementary good source for understanding its design choices is the
[BlocksDS issue tracker](https://github.com/blocksds/sdk/issues?q=is%3Aissue),
where many were and continue to be openly discussed.

## 1. Standardized binary build of the GCC compiler

Existing builds of GCC used for embedded development come with various, often
not fully matching C/C++ standard libraries. The compatibility issues and
added support headache make relying on user-provided GCC a non-viable option.

Conversely, requiring the user to build GCC is a long and not-too-easy process
that doesn't give big advantages over providing binary builds. It would make
setting up the SDK a more fragile and time-consuming process.

LLVM/Clang has also been evaluated, but as of writing it (LLVM 18) appears to
generate worse-performing machine code on NDS than GCC (13). This has not been
extensively tested, however.

Therefore, in order to take advantage of per-CPU optimizations and to have a
robust toolchain, it has been decided to standardize on a custom binary build
of GCC. For this purpose, BlocksDS relies on a courtesy build provided by the
[Wonderful toolchain](https://wonderful.asie.pl)'s infrastructure.

## 2. Easy toolchain modification and rebuilds

Often, SDKs come with pre-built libraries - users only need to build them
manually if they want to modify them. It is the more convenient route for
most, and the Wonderful toolchain's infrastructure currently provides it
for BlocksDS users.

However, part of the reason for creating this SDK was to show users how easy it
actually is to modify any part of the code in your program. As such, one can
easily build (`make`) and use (`export BLOCKSDS`) a custom variant of the
BlocksDS SDK. This is a quick process (under a minute on most modern
machines for a full rebuild) which allows hands-on tinkering with it. 

The exceptions are `picolibc` (standard C library) and `libstdc++` (standard
C++ library), which are fairly difficult to build reliably. They are, as such,
provided by the toolchain.

## 3. Using picolibc over newlib

The C library [picolibc](https://github.com/picolibc/picolibc) is used in
BlocksDS instead of [newlib](https://sourceware.org/newlib). The reasons are:

- Clearer licensing: The maintainer of `picolibc` has ensured that all of the
  code in the library has consistent BSD-style licensing. Even though non-BSD
  code was not linked in a typical embedded build, it provides additional peace
  of mind.

- Standard thread-local storage: `newlib` utilizes a custom re-entrancy
  structure for those C library functions which have thread-dependent state.
  Conversely, `picolibc` uses standard thread-local storage mechanisms
  provided by the compiler.

- Additional optimizations: `picolibc` modifies many aspects of `newlib` which
  help reduce code size and thus memory usage. In particular, `tinystdio` is
  a much leaner implementation of POSIX-compatible standard I/O which omits
  features not typically necessary in an embedded environment.

- Compatibility: `picolibc` is a fork of `newlib`, and so is largely compatible
  with it at the user application level.

The main disadvantage I've seen is that the documentation of `picolibc` about
porting it to a new system is worse than the one of `newlib`.

Other C library alternatives with more convenient licensing have been
evaluated, such as [PDCLib](https://github.com/DevSolar/pdclib/), but as of
writing (May 2024) have been deemed insufficiently complete for production
use in the context of BlocksDS.

## 4. Standard C library port

`picolibc` only provides the generic functionality of the standard C library.
For example, it provides versions of `memset()` or `strlen()` that are
functional. However, it can't access any OS services, so functions like
`malloc()` or `fopen()` don't work right away. It is needed to port them to
the platform.

`libnds` is the library that has the drivers to access the SD card, that
provides `argc` and `argv` to `main()`, and that knows where to locate the
heap memory used by `malloc()`.

For example, for `malloc()` to work, `picolibc` expects the port to provide
a function called `sbrk()`. This function needs to get information from
`libnds` to work. The glue code between `picolibc` and `libnds` is in
`libnds`, in `source/arm9/libc`.

The extent of this support is documented in [this document](../../guides/libc).

The reason to keep this as a separate library, instead of adding it to
`picolibc` as a target, is to make updating `picolibc` mostly independent
from refactors in `libnds`.

## 5. Filesystem support

This section will describe how the filesystem support has been implemented in
libnds. Check [this document](../../guides/filesystem) if you're interested
in the C standard functions that are supported.

Filesystem support requires 3 things:

- Something that provides POSIX file I/O functions, like `fopen()`. This
  is done by `picolibc`.

- Something that reads and writes raw bytes from the SD card. This is done
  by `libnds`.

- Something that understands the raw bytes read from the SD card and interprets
  it as a FAT filesystem. This is done by a modified version of
  [Elm's FatFs library](http://elm-chan.org/fsw/ff/00index_e.html), which is
  included in `libnds` as a submodule.

Also, it is needed to provide glue code between the 3 components. For example:

`picolibc` provides `fopen()`, and expects the user to implement `open()`,
which should work like the Linux system call `open`. `open()` must have code
that calls functions in FatFs to do the right thing. In this case, `open()`
translates its arguments to arguments that `f_open()` from FatFs can
understand.

Internally, `f_open()` requires a function called `disk_read()`, which calls
`libnds` functions to read raw bytes from the SD card. Reading raw bytes is
complicated. If you're running the code on a DSi, and you want to read from the
internal SD card, you need one specific driver. If you are running the code from
a DS slot 1 flashcart, for example, the instructions of how to read from the SD
card are provided as a DLDI driver. `f_open()` must determine the location of
the file (based on the filesystem prefix, `fat:` or `sd:`) and use DLDI
driver functions or DSi SD driver functions accordingly.

## 6. NitroFS implementation

When creating a game, it is needed to add a lot of assets such as graphics and
music. Initially, most people just include them in their ARM9 binary, but this
is a bad idea. ARM7 and ARM9 binaries are loaded into RAM. There are only 4 MiB
of available memory (actually, a bit less than that, some RAM is used for things
like a hook to exit to the loader). The ARM9 is loaded in full to RAM. On top of
that, you also need RAM for your program to work. This means that, in most
cases, you're limited to 1 or 2 MiB binaries. This isn't enough for larger
projects. There is the option to provide a folder with all your assets and tell
your users to copy it to their SD card, but this is messy.

The solution is to append a filesystem to the .nds file. On the DS platform,
a filesystem format called NitroROM is traditionally used. There is a library
that can be used to access this filesystem, called [libfilesystem](https://github.com/devkitPro/libfilesystem)
(formerly known as [Nitrofs](http://blea.ch/wiki/index.php/Nitrofs)). The
problem is that this library's licensing status in unclear. As such, instead of
using this library, BlocksDS has a compatible reimplementation of NitroFS.

Accessing the filesystem itself is tricky.

Commercial games access their own data by issuing card read commands. These
only work on emulators and official cartridges. Flashcarts typically rely
on patching specific instruction sequences, which is not viable for homebrew
projects. The solution, instead, is `argv`.

When it is initialized, `NitroFS` checks if `argv[0]` has been provided and
if it can be opened. `argv[0]` is a path to the executable .nds file being
run. For example, it may look like `fat:/games/my-game.nds`.

First, `NitroFS` will try to open the file using `FatFs`. If it can be
opened, whenever `fopen()` is called with a path that starts with `nitro:/`,
`FatFs` will read blocks from the file in `argv[0]` with `fseek()` and
`fread()`. This route is typically used on cartridges and to read from SD
cards.

If this fails, the Slot-2 cartridge memory area is checked. This is provided
solely for compatibility with very legacy execution methods.

If this fails, Slot-1 card read commands will be attempted. This approach is
provided for compatibility with emulators which do not emulate a full FAT
storage device and the DLDI protocol required for homebrew filesystem access.

This system makes it possible to use the bundled filesystem to read assets
in a transparent manner. The developer doesn't need to worry about the method
of execution used for their homebrew; on the vast majority of platforms, it
will be handled transparently.

## 7. Multithreading

The original `libnds` did not support any kind of multithreading. This made it
impossible to fully utilize the CPUs of the NDS during blocking operations.
For example, it wasn't possible to offload file operations to the ARM7 while
the ARM9 continued to execute code; the ARM9 had to be stalled until the
operation was complete.

BlocksDS supports cooperative multithreading in the form of cothreads. By
integrating it with `libnds`, it is possible for functions like `fopen`
or `fread` to switch to a different thread while they are waiting for the
SD card to finish reading a block.

### DLDI on the ARM7

While accessing the internal SD card on the DSi is performed on the ARM7,
DLDI drivers used by cartridges are traditionally executed on the ARM9.
However, as they are executed synchronously, this stalls the CPU whenever
a file operation is performed, prohibiting the use of multithreading.

As a workaround, BlocksDS supports moving DLDI execution to the ARM7 sub
CPU; this should be compatible with many, but not all cartridges. Feel free
to read [this document](../../guides/dldi_arm7) for more information.

