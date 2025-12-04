---
title: 'DLDI in ARM7'
weight: 40
---

## 1. Introduction

DLDI (Dynamically Linked Disc Interface) is an interface designed for homebrew
projects to be able to access the memory of flashcarts (usually micro SD cards)
in a standardized way. https://www.chishm.com/DLDI/

In short, when you build an application, the DLDI stub is added to your
application. Then you and your users have to patch your application with the
DLDI driver your flashcarts. Most flashcarts patch homebrew NDS ROMs, so
normally you don't need to worry about it, it just works. Some old flashcarts
(specially slot-2 flashcarts) don't support this, so you have to patch ROMs
yourself. DeSmuME doesn't autopatch ROMS, for example, while melonDS does.

This is an archive of DLDI drivers: https://github.com/DS-Homebrew/DLDI

## 2. Why is DLDI in ARM7 important?

Reading files with DLDI is very slow because reading from SD cards is very slow.
It's not limited to DLDI. Regular cartridge read commands (like the ones used by
offical games) are also slow. Traditionally this process happens in the ARM9.

This means that, if you have a game that has a 3D open world, and you want to
move around the world while loading and unloading models and textures from the
filesystem you can't do it in the same CPU. The ARM9 is too busy with the game
logic, it can't just stop until it has finished reading a file.

The fix is to use the ARM7 to do the reads while the ARM9 is doing other things.
For this you can use the cooperative multithreading (cothread) system of libnds.
All you need to do is to start as many threads as you want. Regular C functions
like `fopen()` and `fread()` switch threads automatically when they need to wait
for the ARM7 to read data. Any thread that doesn't read from the filesystem
needs to call `cothread_yield()`, `cothread_yield_irq()` or
`cothread_yield_signal()` to switch threads. In practice, it's pretty easy to
do. Check [this example](https://github.com/blocksds/sdk/tree/master/examples/filesystem/async_loading)
to see how to load two different files using two threads at the same time.

## 3. Supported flashcarts

The main drawback of this approach is that not all flashcarts support this. Some
DLDI drivers have been built for the ARM9 and will crash in the ARM7, for
example. This is why DLDI in ARM7 isn't enforced. The current code supports
using DLDI from both ARM9 and ARM7 without rebuilding the application.

## 4. Adding support to a DLDI driver

All you need to do is to make sure that your code is built for the ARM7 instead
of the ARM9. Use `-mcpu=arm7tdmi` instead of `-mcpu=arm946e-s+nofp`.

Then add the following flag to the features entry of your DLDI header:

```c
#define FEATURE_ARM7_CAPABLE    0x00000100
```

BlocksDS's fork of libnds will detect this flag and switch to using DLDI from
the ARM7.

## 5. Using DLDI with BlocksDS

If your application doesn't care about which CPU uses DLDI, you don't have to do
anything. Any driver marked with `FEATURE_ARM7_CAPABLE` will run from the
ARM7, and any driver without the mark will run from the ARM9. If you want to
force a certain CPU to do it, you can do it this way:

```c
#include <nds/arm9/dldi.h>

// Set the DLDI mode
dldiSetMode(DLDI_MODE_ARM9); // or DLDI_MODE_AUTODETECT or DLDI_MODE_ARM7

// Get the current mode. This is set to DLDI_MODE_ARM9 or DLDI_MODE_ARM7
// after initializing the FAT system.
DLDI_MODE dldi_mode = dldiGetMode(void);
```

## 6. Supported flashcarts

This is a list of flashcarts that have been reported to work. Please, test your
flashcart using
[this test](https://github.com/blocksds/sdk/tree/master/tests/filesystem/dldi_arm9_arm7)
and report your findings.

flashcart       | ARM9 | ARM7 | Notes
----------------|------|------|------
Acekard 2i      | Yes  | Yes  |
SuperCard DSTWO | Yes  | Yes  |
Original R4     | Yes  | Yes  |
