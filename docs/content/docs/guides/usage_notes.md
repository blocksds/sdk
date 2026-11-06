---
title: 'Usage notes'
weight: 3
---

### 1. Introduction

This is just a list of tips and notes about how to use BlocksDS.

### 2. Default ARM7 cores

Most projects need some sort of audio and WiFi support on the ARM7, but they
don't need any custom code. BlocksDS provides a few ARM7 cores with different
combinations of libraries:

| File                    | Usage           | Notes
| ---                     | ---             | --- |
`arm7_dswifi.elf`         | DSWiFi          |
`arm7_dswifi_libxm7.elf`  | DSWiFi + LibXM7 |
`arm7_dswifi_maxmod.elf`  | DSWiFi + Maxmod |
`arm7_libxm7.elf`         | LibXM7          |
`arm7_maxmod.elf`         | Maxmod          | Default core for AMR9 projects
`arm7_minimal.elf`        | Only libnds     |

Debug versions are also available, just add a `_debug` to the basename, such as  `arm7_dswifi_debug.elf` or `arm7_dswifi_libxm7_debug.elf`.

If you try to use DSWiFi, but you don't use a core that supports DSWiFi, the
ARM9 will hang when it tries to communicate with the ARM7.

They are all installed to `$BLOCKSDS/sys/arm7/main_core/*.elf`. If you want to
select a different core, edit the path of `ARM7ELF` in your Makefile to select
the right one.

Note: There is also `$BLOCKSDS/sys/default_arm7/arm7.elf`, which is kept there
for backwards compatibility with older Makefiles. This is the same as the DSWiFi
+ Maxmod core.

Remember to give credit correctly according to the libraries you have present in
the ARM7, even if you aren't using them!

### 3. Section annotations in filenames

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

### 4. Optimizing DTCM usage

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

Users can also designate variables to be placed in DTCM. BlocksDS places them
after the user stack like this:

    +-----------------------------------+--------------------------------------+
    |             Main RAM              |               DTCM                   |
    +-----+--------------------+--------+--------+-------+---------------------+
    | App |        Heap        | Unused | Unused | Stack | User DTCM variables |
    +-----+--------------------+--------+--------+-------+---------------------+

This means that, no matter how many user variables are placed in DTCM, the stack
can still grow into main RAM.

This has a small problem: the linker can't allocate variables starting from the
end of a section, so the size reserved for DTCM user variables needs to be
hardcoded by the user. Then, variables are added to that reserved space from the
bottom. Whatever space is left is available to be used as a DTCM heap.

You can adjust the size of this section by manually setting the value of
`__dtcm_data_size` to the right value for your application (by default it's 1024
bytes). There are two ways to set it:

1. Edit your Makefile and the following to `LDFLAGS`:

   ```make
   LDFLAGS := -Wl,--defsym,__dtcm_data_size=1024
   ```

2. Add a file to your project with the value of the symbol. For example, create
   a file called `dtcm_size.s` with the following contents:

   ```asm
   .global __dtcm_data_size
   .equ __dtcm_data_size, 1024
   ```

If you try to place more variables in DTCM than the available space, the linker
will print an error telling you to increase the size.

You can check [this test](https://codeberg.org/blocksds/sdk/src/branch/master/tests/system/dtcm_fixed_size)
to experiment with this value.

### 5. Using alternative memory layouts

#### ARM7 options

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
Pass option `-h 0x200` to `ndstool` to force the ROM to be NDS-only and remove
DSi code.

#### ARM9 options

They can be selected by changing the `.specs` file used in the Makefile:

- DS layout: `ds_arm9.specs`

- DSi layout: `dsi_arm9.specs`
