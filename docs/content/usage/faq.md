---
title: Frequently asked questions
weight: 30
---

## General questions

### I've just installed BlocksDS, now what?

The first step is to build one of the examples included with BlocksDS. If you
have installed the `thirdparty-blocksds-docs` package with `wf-pacman`, the
examples are located in `/opt/wonderful/thirdparty/blocksds/core/examples/`.
From there, for example, go to `graphics_3d/volumetric_shadow` and type make.
This will generate a NDS ROM that you can try with an emulator or a flashcart.

You can also try one of the open source projects made with BlocksDS that are
listed [here](https://github.com/blocksds/awesome-blocksds).

Unfortunately, there isn't any tutorial on how to develop NDS applications or
games from scratch. Your best bet is to take a look at the examples and to ask
other developers if you get stuck. Check the "getting support" section of the
documentation [here](../../introduction/support).

You can also check the online mirror of the documentation of all libraries
included in BlocksDS [here](../../).

If `libnds` is too low-level for you, you can check high-level libraries like
[Nitro Engine](https://github.com/AntonioND/nitro-engine) or
[NightFox’s Lib](https://github.com/knightfox75/nds_nflib). They don't have
official tutorials either, but they also come with lots of examples you can use
as reference, and it may be easier to get started with NDS development with
them.

### Where can I find low-level NDS hardware documentation?

The main resource is GBATEK; some additional details are documented in the
GBATEK addendum:

- https://problemkaputt.de/gbatek.htm
- https://melonds.kuribo64.net/board/thread.php?id=13

### What emulators are recommended to test my code?

The recommended emulators are [melonDS](https://melonds.kuribo64.net/),
[DeSmuMe](https://github.com/TASEmulators/desmume/releases) and
[no$gba](https://problemkaputt.de/gba.htm). There is more information about
debugging NDS programs [here](../debugging).

### When I load my game on my flashcart, it cannot find NitroFS files.

For NitroFS to work, the NDS ROM needs to know where the ROM itself is located.
On emulators, this can be done by using the official cartridge access protocol;
however, flash cartridges tend to not implement those commands.

Flashcarts traditionally provide a DLDI driver, which allows access to its SD/CF
card or built-in flash storage, including the `.nds` file on said storage. It is
common for modern flashcart software to automatically patch homebrew with the
correct driver. However, this by itself doesn't give the homebrew program
information on *where* the right `.nds` file is.

On DSi consoles, we have a similar issue: while the driver for the DSi's SD card
slot is included in homebrew software, we don't always know where the file is
located.

To solve this problem, the `argv` protocol was defined as a way to pass arguments
to the homebrew program. Unfortunately, many old loaders don't support this
protocol; first, make sure you're running on the newest firmware for your
flashcart.

A good way to avoid this problem altogether is to use the [NDS Homebrew Menu](https://github.com/devkitPro/nds-hb-menu/releases),
which is capable of correctly launching modern homebrew.

Download the `.zip` archive and copy `BOOT.NDS` to your SD card/storage. You can
rename this file to a name that you can remember. Once you have it on the cartridge,
launch the Homebrew Menu, then launch your homebrew program with it. NDS Homebrew
Menu will make sure that the loaded ROM receives the information it needs.

## Feature support

### How well is Wi-Fi supported?

Wi-Fi support in BlocksDS is currently limited. It comes with `DSWiFi`, a
library that provides Wi-Fi support. However, it doesn't support DS-to-DS
direct communications or the new DSi-only features.

It supports connecting to an access point with your DS, even using the access
points configured in the firmware of the DS.

Adding support for this involves a lot of effort, which is the main reason why
there is no support for it at the moment.

Related issues:

- https://github.com/blocksds/sdk/issues/61
- https://github.com/blocksds/sdk/issues/64

### How well is the Teak DSP supported?

The DSi comes with an additional general-purpose CPU apart from the ARM9 and
ARM7. This CPU has an unusual architecture (TeakLite II) which isn't supported
in any open source or freely available toolchain.

There is an experimental LLVM port, but it's stuck on an old version of LLVM
and it requires extensive LLVM patches to support an environment where
`CHAR_BIT` != 8 (chars are 16-bit on the Teak).

For more information, check this link:

https://www.embecosm.com/2017/04/18/non-8-bit-char-support-in-clang-and-llvm/

Related issues:

- https://github.com/blocksds/sdk/issues/93

### How well is the DSi camera supported?

Initial support exists for the DSi's built-in cameras. However, the Aptina chip
features many configuration features, for which no high-level functions are
currently provided.

Related issues:

- https://github.com/blocksds/sdk/issues/41

### Which external peripherals are supported?

BlocksDS provides built-in libraries for a variety of external peripherals:

- [Slot-2 RAM cartridges](https://blocksds.skylyrac.net/docs/libnds/slot2_8h.html)

  - Memory Expansion Pak
  - Most other cartridges

- [Slot-2 gyroscope](https://blocksds.skylyrac.net/docs/libnds/slot2gyro_8h.html)

  - GBA WarioWare

- [Slot-2 rumble](https://blocksds.skylyrac.net/docs/libnds/rumble_8h.html)

  - GBA WarioWare, Drill Dozer
  - DS Rumble Pak
  - Many other cartridges

- [Slot-2 solar sensor](https://blocksds.skylyrac.net/docs/libnds/slot2solar_8h.html)

  - GBA Boktai 1, 2, 3

- [Slot-2 tilt sensor](https://blocksds.skylyrac.net/docs/libnds/slot2tilt_8h.html)

  - GBA Yoshi, Koro Koro Puzzle

- [DS Motion Card](https://blocksds.skylyrac.net/docs/libnds/ndsmotion_8h.html)

  - DS Motion Card
  - DS Motion Pak
  - Motion Pack (bundled with Tony Hawk Motion)
  - Other cartridges

- [Slot-2 Easy Piano](https://blocksds.skylyrac.net/docs/libnds/piano_8h.html)

- [Slot-2 Guitar Grip](https://blocksds.skylyrac.net/docs/libnds/guitarGrip_8h.html)

- [Slot-2 Taito DS Paddle Controller](https://blocksds.skylyrac.net/docs/libnds/paddle_8h.html)

Additional peripheral support is provided by third-party libraries:

- dserial: [Slot-1 serial card](https://github.com/asiekierka/dserial)

- nrio-usb: [Slot-2 USB interface](https://github.com/asiekierka/nrio-usb-examples/)

## Build process

### Why are ELF files used for ndstool?

ELF files are a standard format used by open source toolchains for applications.
They are very flexible. With them it's possible to define parts of the code that
are only loaded in DSi mode, and ignored in DS mode, for example. They are also
supported by debuggers, so it is possible to use `gdb` with emulators like
melonDS and DeSmuMe to debug your games.

There is more information [here](../../technical/build_process).

### Why is libnds required to build CPU binaries?

The initialization code of both CPUs requires functions that are present in
libnds. Some of them are too complicated to be kept in the initialization code
in the `crt0` file, like the MPU setup code.

The license of `libnds` is Zlib, which means that you don't need to give any
kind of attribution to its authors, and it doesn't impose any restriction on how
you distribute your binaries and source code.

There is more information [here](../../technical/build_process).

### Why is a custom C library required?

BlocksDS uses `picolibc` as a C library. `libnds` and `picolibc` are very
tightly coupled so that it's possible to use standard POSIX functions like
`fopen()` or `time()` from NDS programs. While it is possible to replace the C
library or `libnds`, it requires a lot of effort to make them work together.

There is more information [here](../../technical/build_process).

### Why do the BlocksDS makefiles pass specs files to GCC?

The `.specs` files' role is to provide configuration flags for GCC. Many
compiler arguments have been moved here in order to allow changing them with GCC
and BlocksDS updates without causing downstream breakage to Makefiles provided
by end-user projects.
