---
title: 'Dynamic libraries'
weight: 20
---

## 1. Introduction

BlocksDS supports loading dynamic libraries at runtime. This document explains
how the system works, its features and its limitations. The process will be
explained in more detail below, but this is a short explanation:

In BlocksDS, a dynamic library is created by compiling files and generating an
ELF file with relocations. This ELF file is converted into DSL with `dsltool`.
DSL files can be loaded from the main binary using `dlopen()` in a similar way
as `.so` files in Linux. Then, `dlsym()` can be used to access functions and
other symbols from the DSL file.

This system makes it possible to have NDS ROMs with a lot of code that doesn't
fit in RAM, or with external plugins that can be developed by users and loaded
from the SD card of their flashcard or DSi slot.

If you want to jump right to the code, check the examples
[here](https://github.com/blocksds/sdk/tree/master/examples/dynamic_libs).

The system used by BlocksDS is heavily inspired by the one used by
[libdragon](https://github.com/DragonMinded/libdragon/). There is more
information about the system in the libdragon
[wiki](https://github.com/DragonMinded/libdragon/wiki/DSO-(dynamic-libraries)).

## 2. Creating libraries

The first step is creating the libraries you want to load. A library can contain
graphics, data, audio, source code, or anything that a regular binary would
have. All of this is built as an ELF file with `0x00000000` as base address.
Normally, this would be useless because the load address of this ELF file won't
be `0x00000000` when it gets loaded, but this ELF file is generated with
relocations (`-Wl,--emit-relocs`). Relocations are just notes in the ELF file
that tell the ELF loader which addresses to patch in the code so that the code
can run when loaded at a different address than `0x00000000`.

ELF files also contain a list of symbols. They are just a string (the original
name of the symbol/function) and an address. All symbols in the library are
marked as "hidden" by default with compiler option `-fvisibility=hidden`. If you
want to make them available for the main binary, you need to use
`__attribute__((visibility ("default")))`.

Linker option `-Wl,--unresolved-symbols=ignore-all` is also used when generating
the ELF file. This option makes the compilation succeed even if there are
symbols used in the dynamic library that are missing from the library itself.
This is useful if the library wants to use symbols from the main binary, the one
loading the library. For example, if the library uses `malloc()`, it needs to be
able to access the `malloc()` from the main binary. This symbol will be marked
as "unknown" when the ELF file is generated. This also applies to `libgcc`
functions. For example, when the source code divides two integers, it calls
`__aeabi_idiv()`, found in `libgcc`. If this function isn't already in the main
binary, the library won't be able to divide integers!

The last unusual option is `-Wl,--target1-abs`. Constructor pointers in the
`init_array` generate relocations of type `R_ARM_TARGET1`. This can be
interpreted as either `R_ARM_REL32` or `R_ARM_ABS32` depending on the target
architecture. By default, `arm*-*-elf` and `arm*-*-eabi` use `rel` relocations,
and only `arm*-*-symbianelf` uses `abs` relocations. To simplify libnds we can
use the `--target1-abs` switch, which lets us not implement `R_ARM_REL32`. You
can find more information [here](https://sourceware.org/binutils/docs/ld/ARM.html).

## 3. Converting ELF files to DSL

Converting the ELF file to DSL is a process that involves reading all the
sections and symbols from the ELF file and removing all information that isn't
required to be loaded by libnds. This is done purely as a memory saving step.
[`dsltool`](https://github.com/blocksds/sdk/tree/master/tools/dsltool) is the
tool in charge of this process.

A DSL file contains a list of sections and a list of symbols. The sections are
defined as "progbits" (code and variables with initial data), "nobits"
(variables without initial values) or "relocations". Normally, there should be
one of each, and they should be saved sequentially as they are found on memory
(relocations aren't stored in memory, so it doesn't matter in which index the
section is). The sections are just a copy of the sections from the ELF file.

The list of symbols is taken from the ELF file, but any private symbols are
removed to save space. Relocations are created pointing to a symbol index, so
they need to be modified to point to the new symbol indices.

Relocations are only modified to use the new symbol indices, but `dsltool`
checks that the relocations in the ELF file are supported by `dlopen()` to
prevent runtime loading errors. It's better to detect this kind of errors at
build time.

Finally, unknown symbols have to be resolved by `dsltool` at this step. If you
have access to the ELF file of the main binary, you can also pass it to
`dsltool`, and `dsltool` will search the main binary for them. If they are
found, the address of the symbols will be added to the DSL file.

If the main binary ELF file isn't available, this process can't happen. It is
still possible to call functions from the main binary if the library has a
function that can receive function pointers from the main binary. The main
binary can send the addresses of any required function, and the library can use
them at that point.

A game that has a plugin system may not want to provide the ELF file of the main
binary for developers creating plugins. The game could then create a system that
provides different permissions to different libraries. For example, all plugins
could receive the pointers to `malloc()` and `free()`, but only privileged
plugins would receive pointers to `fopen()` and related functions.

After a DSL file is built, it can be stored in either nitroFS or the SD card.

## 4. Loading DSL files

The main binary needs to initialize NitroFS or FAT (`nitroFsInit()` or
`fatInitDefault()`). Then, it has to call `dlopen()` with the path to the DSL
file.

`dlopen()` will first read the file and use `malloc()` to allocate enough memory
for progbits and nobits sections. Once sections are loaded, the symbols table is
loaded. This table is required for `dlsym()` to work later too. Once all
sections and symbols are loaded, relocation addresses are patched. In the end,
the size of a dynamic library in RAM is the size of the progbits and nobits
sections, plus the symbol table. When everything is loaded, and relocations are
handled, the global constructors of the library are called.

When `dlsym()` is called, the symbol table is searched for that string. If the
string is found, and the symbol is marked as public, it will return the address
to that symbol so that the main binary can use it.

Note that to obtain pointers to C++ symbols with `dlsym()` you need to use the
mangled names of the symbols. You can check the symbols of the name with:

```sh
readelf -a --wide path/to/library/library.elf
```

If you don't have `readelf` in your system, you can use the one from Wonderful
Toolchain:

```sh
/opt/wonderful/toolchain/gcc-arm-none-eabi/arm-none-eabi/bin/readelf
```

Once the main binary is done with the library, it can call `dlclose()`. This
will first call all global destructors of the library and then free all memory
used by it.

You can find the code of the functions in this file in
[libnds](https://github.com/blocksds/libnds/blob/master/source/arm9/dlfcn.c).

## 5. Advantages over overlays

Overlays are supported in other SDKs, and they are just parts of code that are
built as part of the main binary, but use the same space in memory. For example,
if the code of enemy 1 and enemy 2 are never used at the same time, it is
possible to ask the linker to place the code in the same area in RAM. The
program is in charge of loading the code of enemy 1 or enemy 2 depending on what
it needs to use at the time.

Overlays behave like any other code of the binary, their only cost is that they
need to be copied to RAM when they are going to be used. However, they are
limited in the sense that it isn't possible to load both enemy 1 and enemy 2
at the same time. They both need to be loaded at the same address. It is
possible to support combinations of overlays if they are built at different
addresses, but that becomes a very complicated process very quickly depending on
the combinations of overlays you need.

Dynamic libraries can be loaded at any address without limitations, and without
any changes to the linkerscript of the application. However, this flexibility
has a cost. Any access to functions or variables of the library needs to happen
through a pointer, so there is always the cost of an additional indirection
compared to overlays. If the library is full of very small functions, this cost
may be noticeable. If the functions are big, this cost won't be noticeable.

## 6. Debugging dynamic libraries

When you're in a GDB session, run the following command to load the symbols of
the dynamic library:

```
add-symbol-file path/to/dynamic.elf -s .progbits <load_address>
```

You can obtain the address by calling the DS-specific function `dlmembase()`.

## 7. Limitations

- Libraries must use symbols that are present in the main binary. It isn't
  possible for a binary to use `malloc()` if the main binary doesn't use
  `malloc()`, for example.

  This limitation may change in the future. It may become possible to ensure
  that functions requested by a library are included in the main binary.
  However, this will only work for libraries that are built at the same time as
  the main binary. A game that supports plugins won't have this luxury, as the
  game itself is fixed. Plugins will be limited to whatever the game uses.

- Libraries can't call functions from other libraries. This may be supported in
  the future.

- Libraries may use thread-local variables from the main binary, but they can't
  have their own thread-local variables.
