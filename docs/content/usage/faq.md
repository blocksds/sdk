---
title: Frequently Asked Questions
weight: 30
---

## General questions

### Where can I find information on how to do low-level NDS development?

The main resources are GBATEK and the GBATEK addendums:

- https://problemkaputt.de/gbatek.htm
- https://melonds.kuribo64.net/board/thread.php?id=13

### What emulators are recommended to test my code?

This is documented [here](../debugging).

### When I load my game on my flashcart, it doesn't find NitroFS files.

For NitroFS to work, the NDS ROM needs to know where the ROM itself is located.

On emulators, this is easy: The ROM is accessed with official NDS cartridge
commands.

On flashcarts we need to use the right DLDI driver to read from the SD card of
the flaschard, but we also need to know where the NDS ROM file is located in the
filesystem. The loader of homebrew NDS ROMs needs to patch the ROMs with the
right DLDI driver, and it needs to pass the absolute path to the ROM in the
filesystem.

On DSi we have a similar issue: we always have the driver to read
from the SD card slot of the DSi, but we don't know where the file is located.

To solve this problem, the `argv` protocol was created. Unfortunately, many old
loaders don't support this protocol. You can try to get a firmware update for
your flashcart and try again. If that doesn't work, you can use the NDS Homebrew
Menu: https://github.com/devkitPro/nds-hb-menu/releases

Download the zip archive and copy `BOOT.NDS` to your flashcart. You can rename
this file to a name that you can remember. Once you have it in your flashcart,
use your flashcart firmware to boot NDS Homebrew Menu, and load the original NDS
ROM with it. NDS Homebrew Menu will make sure that the loaded ROM receives the
information it needs.

## Unsupported hardware

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

## Build process

### Why are ELF files used for ndstool?

ELF files are a standard format used by open source toolchains for applications.
They are very flexible. With them it's possible to define parts of the code that
are only loaded in DSi mode, and ignored in DS mode, for example. They are also
supported by debuggers, so it is possible to use `gdb` with emulators like
melonDS and DesMuMe to debug your games.

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
