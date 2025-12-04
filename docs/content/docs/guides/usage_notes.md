---
title: 'Usage notes'
weight: 5
---

## 1. Introduction

This is just a list of tips and notes about how to use BlocksDS.

## 2. Default ARM7 cores

Most projects need some sort of audio and WiFi support on the ARM7, but they
don't need any custom code. BlocksDS provides a few ARM7 cores with different
combinations of libraries:

| File                    | Usage           | Notes
| ---                     | ---             | --- |
`arm7_dswifi.elf`         | DSWiFi          |
`arm7_dswifi_libxm7.elf`  | DSWiFi + LibXM7 |
`arm7_dswifi_maxmod.elf`  | DSWiFi + Maxmod | Default core for AMR9 projects
`arm7_libxm7.elf`         | LibXM7          |
`arm7_maxmod.elf`         | Maxmod          |
`arm7_minimal.elf`        | Only libnds     |

Debug versions are also available, just add a `_debug` to the basename, such as  `arm7_dswifi_debug.elf` or `arm7_dswifi_libxm7_debug.elf`.

They are all installed to `$BLOCKSDS/sys/arm7/main_core/*.elf`. If you want to
select a different core, edit the path of `ARM7ELF` in your Makefile to select
the right one.

There is also `$BLOCKSDS/sys/default_arm7/arm7.elf`, which is kept there for
backwards compatibility with older Makefiles. This is the same as the DSWiFi +
Maxmod core.

Remember to give credit correctly according to the libraries you have present in
the ARM7, even if you aren't using them!

## 3. Section annotations in filenames

Some projects require specific functions or variables to be placed in specific
memory regions. Some functions may also need to be compiled as ARM instead of
Thumb, which is the default.

It is possible to rename your C and C++ source files to affect the whole file.
For example, a file named `engine.arm.c` will be built as ARM code, and a file
called `interrupts.itcm.c` will be placed in the ITCM memory section.

You can also use the annotations in `<nds/ndstypes.h>`:

- `*.dtcm.*`:  `DTCM_DATA`, `DTCM_BSS`
- `*.itcm.*`: `ITCM_CODE`
- `*.twl.*`: `TWL_CODE`, `TWL_DATA`, `TWL_BSS`
- `*.arm.c`, `.arm.cpp`: `ARM_CODE`

For assembly source files, you can use the `BEGIN_ASM_FUNC` macro and specify
the section in the second parameter:

```asm
#include <nds/asminc.h>

BEGIN_ASM_FUNC my_function itcm

    mov r0, #0
    bx  lr
```

## 4. Optimizing DTCM usage

Applications are loaded at the start of main RAM (code, data sections,
statically-allocated variables, etc). All remaining space is used as heap for
`malloc()` and similar functions.

BlocksDS places the user stack in DTCM. It starts at the end of DTCM and it
grows downwards.

The result is something like this:

    +-----------------------------------+--------------------------------------+
    |             Main RAM              |               DTCM                   |
    +-----+--------------------+--------+------------------------------+-------+
    | App |        Heap        | Unused |           Unused             | Stack |
    +-----+--------------------+--------+------------------------------+-------+

In most cases the heap grows up to a certain address and all space after that
remains unused. Similarly, the stack grows downwards up to a certain address and
leaves some DTCM unused.

This layout allows the stack to grow as much as needed, and even reach main RAM
if DTCM isn't big enough. If this is something that your application expects,
you should use `reduceHeapSize()` to block `picolibc` from using space at the
end of main RAM for functions like `malloc()`. If you don't use this, it's
possible that the stack will overwrite some memory allocated by `malloc()` if it
grows too much. You can use `getHeapLimit()` to check the top of the memory
space that will be used by `malloc()`. This is the result of blocking some space
at the end of main RAM:

    +-----------------------------------+--------------------------------------+
    |             Main RAM              |               DTCM                   |
    +-----+----------+--------+---------+---------+----------------------------+
    | App |   Heap   | Unused | Blocked | Unused  |         Stack              |
    +-----+----------+--------+---------+---------+----------------------------+

However, if an application uses a lot of heap memory and stack it's possible
that the stack and the heap overlap, so you still need to be careful.

There is another problem.

Users can also designate variables to be placed in DTCM. Normally they are
placed at the start of DTCM like this:

    +-----------------------------------+--------------------------------------+
    |             Main RAM              |               DTCM                   |
    +-----+--------------------+--------+---------------------+--------+-------+
    | App |        Heap        | Unused | User DTCM variables | Unused | Stack |
    +-----+--------------------+--------+---------------------+--------+-------+

This means that the stack is now constrained by how much size is taken by
user-defined DTCM variables. Ideally, we'd want the linker to place user DTCM
variables at the end of DTCM and the stack right below them. However, the linker
isn't able to allocate space in a section starting from the end.

BlocksDS has a workaround: Letting the developer hardcode the size reserved for
user DCTM variables. By hardcoding this size the linker can place them at the
end of DTCM and have a much more useful memory layout:

    +-----------------------------------+--------------------------------------+
    |             Main RAM              |               DTCM                   |
    +-----+--------------------+--------+--------+-------+---------------------+
    | App |        Heap        | Unused | Unused | Stack | User DTCM variables |
    +-----+--------------------+--------+--------+-------+---------------------+

To do this you need to manually set the value of `__dtcm_data_size` to the right
size for your application. There are two ways to set it:

1. Edit your Makefile and the following to `LDFLAGS`:

   ```
   LDFLAGS := -Wl,--defsym,__dtcm_data_size=1024
   ```

2. Add a file to your project with the value of the symbol. For example, create
   a file called `dtcm_size.s` with the following contents:

   ```
   .global __dtcm_data_size
   .equ __dtcm_data_size, 1024
   ```

You can check [this test](https://github.com/blocksds/sdk/tree/master/tests/system/dtcm_fixed_size)
to experiment with this value.

## 5. Using alternative memory layouts

### ARM7 options

There are three different layouts provided by BlocksDS that applications can use
for their ARM7 code. They can be selected by changing the `.specs` file in the
ARM7 makefile:

- Default: `ds_arm7.specs`. It expects all code to be in main RAM, and relocates
  it to IWRAM during boot.

- IWRAM: `ds_arm7_iwram.specs`. It expects all code to be in IWRAM from the
  start.

- VRAM: `ds_arm7_vram.specs`. This option places all sections in VRAM (which
  needs to be configured as ARM7 memory by the ARM9) and expects the code to be
  in VRAM from the start.

Note that the IWRAM and VRAM options aren't supported for DSi applications.

### ARM9 options

They can be selected by changing the `.specs` file used in the Makefile:

- DS layout: `ds_arm9.specs`

- DSi layout: `dsi_arm9.specs`
