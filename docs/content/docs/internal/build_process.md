---
title: 'Build process information'
weight: 5
---

### 1. Introduction

NDS ROMs have an ARM9 binary, an ARM7 binary, an optional read-only filesystem,
and a ROM header. The ARM9 and ARM7 binaries are loaded to RAM by the NDS ROM
loader. The filesystem must be read at runtime, and it's where you store
additional graphics, audio, data (and potentially code) that doesn't fit in your
ARM9 and ARM7 binaries. The ROM header has information about where to locate all
the parts of the ROM, as well as other information like the game title and icon.
You can get more information about the header
[in GBATEK](https://problemkaputt.de/gbatek.htm#dscartridgeheader).

This document explains the way BlocksDS builds each component and assembles the
final ROM.

### 2. ARM7 and ARM9 binaries

#### 2.1 ELF sections

The developer provides source code files that are compiled and linked into an
`.elf` file (one per CPU). The source code files may contain code or data as
requested by the developer. This code and data is placed into different sections
as with any other `.elf` file. The most important sections (which use standard
names) are:

- `text`: Code.
- `rodata`: Constant data.
- `data`: Initialized variables.
- `bss`: Uninitialized variables (they start as 0).
- `tdata`, `tbss`: Thread-local `data` and `bss`. Any variable with attribute
  `__thread` is thread-local, which means each thread has access to its own copy
  of the variable instead of all threads using the same variable.

Other interesting (but non-standard) sections are:

- `dtcm`, `sbss`: DTCM is an ARM9 memory region that can be used for data (not
  for code) and which is very fast compared to main RAM. BlocksDS uses it for
  the ARM9 stack. `sbss` is a standard `elf` section name kept for
  compatibility (for example, GBA linker scripts use `sbss` to refer to EWRAM).
- `itcm`, `vectors`: ITCM is an ARM9 memory region that can be used for code
  (and data) and that is also very fast compared to main RAM. `vectors` refers
  to the exception vectors.
- `secure`: This space is reserved for the cartridge secure area, which isn't
  used by homebrew programs.

For more information about how BlocksDS uses the DS memory map check
[this document](../memory_map).

This `elf` file is very useful even after the ROM is built. It can be used with
binutils, for example, to analyse which functions are the biggest ones in your
program (with `nm --size-sort file.elf`). It can also be used to debug games in
emulators that support `gdb`, like melonDS and DeSmuMe.

The DSi complicates all of this a bit more. The DSi has 16 MiB of RAM, while a
regular DS only has 4 MiB. It's very useful for any DSi-compatible game to be
able to not load DSi-related code and data when running on a regular DS.  This
is done by defining other sections that are ignored in a regular DS, and they
are loaded to RAM in a DSi:

- `twl`: Combined `text`, `rodata` and `data` for DSi.
- `twl_bss`: Uninitialized DSi-only variables.

The way the `twl` sections are used is explained below, in the section about
creation of NDS ROMs.

For more information, you can see the full linker scripts used by BlocksDS in
[this folder](https://github.com/blocksds/sdk/blob/master/sys/crts).

Note: In the past, the build process involved creating `bin` files from `elf`
files (with `objcopy`) and using them to build the final ROM. However, `bin`
files generated this way don't contain information about the entrypoint address,
and it isn't possible to determine which parts are only needed for DSi.

### 2.2 Custom linkerscript and crt0 files

Building an ARM7/ARM9 binary for the NDS can be done with a generic toolchain
without any modifications, all you need to do is to provide a custom
linkerscript and `crt0`.

The ARM7 and ARM9 have harcoded memory layouts. Linkerscripts describe the
memory map, and they are used by the linker in the linking step to assign
addresses to all code and data in your program. As the ARM7 and ARM9 have
slightly different memory maps, it is needed to have two different
linkerscripts, one per CPU.

A `crt0` file contains the code that runs right at the start of the program, and
this code:

- Initializes the hardware to a sensible state. This involves configuring up the
  ARM9 MPU (Memory Protection Unit) so that DTCM and ITCM are available and the
  data and instruction caches are enabled. It also involves making sure that
  timers, DMA channels, audio, etc, are all stopped in case the loader didn't do
  it.

- Copies code and data sections to their final locations in physical memory.

- Calls the `libc` functions that initialize the C and C++ runtimes.

- Synchronizes the boot process of the ARM7 and ARM9 so that they are both ready
  before jumping to user code.

- Jumps to `main()`.

It is possible to use BlocksDS without the default `crt0` by passing
`-nostartfiles` to the linker, but then you will be in charge of initializing
the hardware yourself.

### 2.3 Dependency on libnds

It is required to link `libnds` in all programs because of two reasons:

- The `crt0` expects some specific hardware initialization functions to exist
  (this code isn't in the `crt0` itself). `libnds` contains the implementation
  of the functions.

- `libc` and `libnds` are tightly coupled. For example, you can use `fopen()` to
  open a file in the SD card of your flashcard, but that means that `libc` needs
  to talk to `libnds` to do the low-level filesystem handling. `libc`
  understands files, but it doesn't understand filesystem formats, and it
  doesn't know how to read an SD card. `libnds` has code to read SD cards, and
  it integrates `FatFs`, which understands the FAT filesystem format.

If you don't use `libnds` you will need to write your own `crt0`, and either
give up on `libc` integration or implement it yourself.

In general, using `libnds` in all your programs isn't a problem because its
license is `Zlib`, which doesn't impose any restriction on how you distribute
your code or binaries, and it doesn't require any attribution like copyright
notices in your game. You are very welcome to add an acknowledgment to its
authors, but it's not mandatory.

### 2.4 Specs files

Because of all the custom `gcc` options required to build NDS binaries
correctly, it is possible that new options are required after `gcc` updates, or
old options need to be removed, etc. Most of the options can be modified in the
makefile of a BlocksDS project. However, some of this is hidden from the user in
`specs` files located [here](https://github.com/blocksds/sdk/tree/master/sys/crts).
This is done purely for end user convenience and to reduce the amount of
breaking changes to user project Makefiles. You can skip the `specs` files and
copy the build options and definitions to your makefile, but that will force you
to keep track of the changes in BlocksDS and to update your makefiles
accordingly.

The best way to understand how BlocksDS invokes `gcc` is to build a ROM that
uses one of the default makefiles (located
[here](https://github.com/blocksds/sdk/tree/master/sys/default_makefiles/)).
If you build the binary with `make V=` instead of just `make`, the makefile will
print the exact commands used at every step of the build process. You will be
able to see which libraries are used, which compiler options are used, etc.

### 3. Filesystem

The size of the ARM7 and ARM9 binaries is limited to the available RAM of the
console. In order to support bigger games, the NDS ROM can contain a `NitroFS`
filesystem with your graphics, music, etc. That way you can have a smaller CPU
binary and load data from the filesystem whenever it's required.

There is no standalone tool to generate a filesystem image. Developers are
expected to have one or more folders with data and pass them to `ndstool` when
the ROM is created. `ndstool` will generate a filesystem image by combining all
the folders provided by the user.

### 4. Teak DSP binaries

The DSi has another general-purpose CPU that can run user code, a Teak DSP.
Binaries for this CPU are treated as generic data. You can add them to your CPU
binaries, or keep them in the filesystem so that they can be switched at
runtime. Note that this toolchain isn't very mature, so you will need to be very
careful and test your code often if you decide to use it.

### 5. NDS ROM creation

`ndstool` is a program that can take the following and generate a NDS ROM out of
it:

- One ARM9 `elf` file.
- One ARM7 `elf` file. If no file is specified, `ndstool` will use the
  [default ARM7](https://github.com/blocksds/sdk/tree/master/sys/arm7)
  of BlocksDS instead.
- One or more folders to be added to the ROM filesystem. If no folder is
  provided, the filesystem will be kept emtpy.
- One ROM icon (which may be animated for DSi-compatible games). This icon is
  displayed in the NDS firmware menu. It's optional.

`ndstool` reads the `elf` file of each CPU, takes every section in the `elf`
file, and saves it to the NDS ROM as binary data. This includes `twl` sections
as well as regular sections.

It is possible to generate a NDS ROM without any DSi code or data by running
`ndstool` with `-h 0x200`, which will force the NDS ROM header to be DS-only. In
this case, none of the the `twl` sections will be added to the final NDS ROM.
This option also generates a PassMe header; while initially intended for loading
DS homebrew from Slot-2 cartridges, it is also used by some outdated homebrew
application loaders.
