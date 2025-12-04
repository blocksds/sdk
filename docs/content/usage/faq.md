---
title: Frequently asked questions
weight: 30
---

## General questions

### I've just installed BlocksDS, now what?

The first step is to build one of the examples included with BlocksDS. If you
have installed the `blocksds-docs` package with `wf-pacman`, the examples are
located in `/opt/wonderful/thirdparty/blocksds/core/examples/`. From there, for
example, go to `graphics_3d/volumetric_shadow` and type make. This will generate
a NDS ROM that you can try with an emulator or a flashcart.

You can also try one of the open source projects made with BlocksDS that are
listed [here](https://github.com/blocksds/awesome-blocksds).

Unfortunately, there isn't any tutorial on how to develop NDS applications or
games from scratch. Your best bet is to take a look at the examples and to ask
other developers if you get stuck. Check the "getting support" section of the
documentation [here](../../about/support).

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

A good way to avoid this problem altogether is to use the
[NDS Homebrew Menu](https://github.com/devkitPro/nds-hb-menu/releases), which is
capable of correctly launching modern homebrew.

Download the `.zip` archive and copy `BOOT.NDS` to your SD card/storage. You can
rename this file to a name that you can remember. Once you have it on the cartridge,
launch the Homebrew Menu, then launch your homebrew program with it. NDS Homebrew
Menu will make sure that the loaded ROM receives the information it needs.

### Is libnds an outdated library?

Short answer: No. BlocksDS considers libnds a core library of the SDK, and it
receives regular updates and support.

Long answer: The version of libnds included in BlocksDS has received support
since the first release of BlocksDS in 2023. Most of the problems it had before
the creation of BlocksDS have been resolved or mitigated. Some bugs that were
over ten years old have been fixed. Others may remain, but the library is
actively being maintained as part of the BlocksDS SDK. Even if the function
names are the same, there have been a lot of internal changes to add new
features and fix bugs. Many parts of the code have been documented properly
(MPU setup, reset to loader protocol...). So no, libnds can't be considered an
outdated library.

The reality is that libnds is a library with 20 years of history. It started its
life as ndslib in 2005, it was renamed to libnds, and it has had contributions
by dozens of people since then. There hasn't a big push to uniformize coding
style or naming conventions for most of its life. BlocksDS has uniformized the
coding style, but things like function names aren't easily changed (any change
has the potential to break someone's code). This may make it feel a bit weird or
outdated, but it's rare to find projects that are this old and don't have
similar issues.

Another way to see it is that libnds has 20 years of people using it, reporting
bugs and adding features. This is very valuable, and it's a bad idea to drop it
in favour of a new library just because the names of some functions don't follow
the same convention as the names of a different function. You would have the
same problem if you decide to use any two libraries you find online (for
example, SDL and libcurl).

BlocksDS is developed with the idea that it's important to not break the
projects of people already using it, so the old names need to remain available.
Maintaining two sets of functions to do the same thing takes a lot of work, and
it isn't a very productive way to use the limited time that the developers of
BlocksDS have. Incremental changes that makes the library better is great
because they help every current and future user of the library.

## Feature support

### How well is Wi-Fi supported?

DSWiFi now supports all features of the DS and DSi hardware! It supports local
multiplayer without a router, and it supports connecting to the Internet using
open, WEP and WPA2 networks (WPA2 is only supported on DSi because of hardware
limitations!).

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
