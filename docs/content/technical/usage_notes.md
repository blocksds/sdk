---
title: 'Usage notes'
weight: 0
---

## 1. Introduction

This is just a list of tips and notes about how to use BlocksDS.

## 2. Default ARM7 cores

Most projects need some sort of audio and WiFi support on the ARM7, but they
don't need any custom code. BlocksDS provides a few ARM7 cores with different
combinations of libraries:

- DSWiFi + Maxmod (Selected by default in all ARM9-only projects)
- DSWiFi + LibXM7
- Maxmod
- LibXM7
- DSWiFi

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

## 4. Using alternative memory layouts

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
