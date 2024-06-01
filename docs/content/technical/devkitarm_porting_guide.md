---
title: 'Migrating from devkitARM'
weight: 60
---

## 1. Introduction

In most cases, porting devkitARM projects to BlocksDS should be relatively easy.
BlocksDS includes most of the NDS functionality provided by devkitARM. For simple
projects, only minor changes should be required.

## 2. New build system

This is the biggest difference between devkitARM and BlocksDS. Makefiles provided
by devkitARM are more complicated: they call themselves recursively from the build
directory. They also hide a lot of compilation rules from the user, as they are
provided by sub-makefiles in the devkitARM system directory.

The Makefiles of BlocksDS include all available rules so that its behaviour is
easier to understand and customize. This allows for example creating a new build
system based on them, tailored to your project (for example, with CMake or Meson).
They also do not rely on self-recursion.

As a BlocksDS user, you need to edit a few paths and variables, same as with
devkitARM. Open the Makefile of your devkitARM project and check this part (some
variables may be missing if you're not using them):

```make
TARGET      := $(notdir $(CURDIR))   # Name of the resulting NDS file
SOURCES     := source source/common  # Directories with files to compile
INCLUDES    := include               # Directories with files to #include
GRAPHICS    := graphics              # Folder with images and .grit files
MUSIC       := audio                 # Folder with audio files for maxmod
DATA        := data                  # Folder with .bin files
NITRODATA   := nitrofs               # Root of your NitroFS filesystem
```

Copy the Makefiles from the `rom_arm9_only` or `rom_combined` to your project,
and open it. You have to copy the values to the following part, and leave them
empty if you aren't using them:

```make
SOURCEDIRS  := source
INCLUDEDIRS := include
GFXDIRS     := graphics
BINDIRS     := data
AUDIODIRS   := audio
NITROFSDIR  := nitrofs
```

Important notes:

- `SOURCEDIRS` searches all directories recursively. If you don't
like this behaviour, go to the `SOURCES_S`, `SOURCES_C` and `SOURCES_CPP` lines
and add `-maxdepth 1` to the `find` command.
- Paths to directories that are outside of the root of the project is not 
supported.

Note that `TARGET` is not part of this group. The top of the Makefile has this
other group of variables that you can also set to your own values:

```make
NAME          := template_arm9     # Name of the resulting NDS file

# Banner and icon information
GAME_TITLE    := Combined ARM7+ARM9 template
GAME_SUBTITLE := Built with BlocksDS
GAME_AUTHOR   := github.com/blocksds/sdk
GAME_ICON     := icon.bmp
```

Once this has been adapted to your desired values, you will need to link with
the libraries used by your program.

This is how it looks like in a devkitARM project:

```make
LIBS := -ldswifi9 -lmm9 -lnds9 -lfat -lfilesystem

LIBDIRS := $(LIBNDS)
```

This would be the equivalent in a BlocksDS project:

```make
LIBS    := -ldswifi9 -lmm9 -lnds9
LIBDIRS := $(BLOCKSDS)/libs/dswifi \
           $(BLOCKSDS)/libs/maxmod \
           $(BLOCKSDS)/libs/libnds
```

You can remove the dswifi or maxmod libraries if you aren't using them.

The reason for this additional complexity with `LIBS` and `LIBDIRS` is to allow
the user as much flexibility as possible when mixing and matching libraries.

## 3. Filesystem libraries

devkitARM and BlocksDS also heavily differ with regards to the structure of
their filesystem access libraries. For most users, this should not lead to major
code chagnes.

devkitARM uses `libfat` and `libfilesystem`, connected to a modified version of
its C library `newlib` through the `devoptab` interface. These are devkitPro's
additions, and are not replicated in BlocksDS.

Instead, BlocksDS uses the `picolibc` C library's "tiny" stdio implementation.
In addition, to cut down on RAM and code size, it omits the `devoptab` interface.
AS most homebrew does not modify the device list, this should not affect them.
It also uses a modified version of [Elm's FatFS library](http://elm-chan.org/fsw/ff/00index_e.html)
in place of libfat, as well as a custom implementation of NitroFS.

To adapt, you have to remove `-lfat` and `-lfilesystem` from `LIBS` in your Makefile.
As all filesystem support is included in `libnds`, no additional libraries are required.

From the point of view of the source code, you can use the same includes as when
using `libfat` and `libfilesystem`:

```c
#include <fat.h>
#include <filesystem.h>
```

They provide `fatInitDefault()` and `nitroFSInit()`. They should be compatible
with the ones in the original libraries.

Beyond the limitations listed above, filesystem support should work identically.
Please report any behaviour that isn't the same. If any other functionality your
homebrew program requires is missing, please report that as well.

## 3a. Note about readdir() compatibility

`readdir()` returns a `struct dirent` pointer with the field `d_type`.
This field can be used to determine if an entry is a directory or a file. I've
seen that some programs use it like this:

```c
struct dirent *cur = readdir(dirp);
if (cur->d_type & DT_DIR)
    printf("This is a directory\n");
else if (cur->d_type & DT_REG)
    printf("This is a file\n");
```

However, this is incorrect. The right way to compare it is by checking if the
value is equal:

```c
struct dirent *cur = readdir(dirp);
if (cur->d_type == DT_DIR)
    printf("This is a directory\n");
else if (cur->d_type == DT_REG)
    printf("This is a file\n");
```

## 4. Integer versions of stdio.h functions

The `newlib` C library provides faster and smaller integer versions of `stdio.h`
functions, such as `iprintf()` or `siscanf()`. These are not provided by
`picolibc`. Replace any calls to them by the standard names of the functions:
`printf()`, `sscanf()`, etc.

In `newlib`, including both `iprintf()` and `printf()` in the same codebase led
to including both versions of the relevant `stdio.h` functions, unnecessarily
increasing code size. `picolibc` opts for a different approach - it allows you
to decide, project-wide, if you want to use integer-only or float-compatible
versions of the `printf()` and `scanf()` functions. Note that by default, for
compatibility reasons, the float-compatible versions are provided.

`picolibc`'s printf/scanf replacement functionality is documented further in
[the optimization guide](../optimization_guide).

## 5. libnds touch screen and keyboard handling

`scanKeys()` updates the internal state of the key handling code. This is then
used by `keysHeld()` and `keysDown()`.

However, BlocksDS also requires the user to call `scanKeys()` before any of the
following functions: `touchRead()`, `keyboardGetChar()`, `keyboardUpdate()` and
the deprecated `touchReadXY()`.

This is unlikely to be a problem in most projects, as the normal thing to do is
to both scan the keys and read the touchscreen status, not just read the
touchscreen.

The reason is that the functions `scanKeys()` and `touchRead()` aren't
synchronized in any way, which creates a race condition. Consider this code:

```c
while (1)
{
    scanKeys();
    if (keysHeld() & KEY_TOUCH)
    {
        touchPosition touchPos;
        touchRead(&touchPos);
        printf("%d, %d\n", touchPos.px, touchPos.py);
    }
    swiWaitForVBlank();
}
```

The state of the X and Y buttons, as well as the state of the touch screen, is
passed from the ARM7 with a FIFO message. This message can technically happen in
between `scanKeys()` and `touchRead()`, which means that it's possible that
`keysHeld() & KEY_TOUCH` is true, but the coordinates read by `touchRead()` are
(0, 0) because the user has stopped pressing the screen right at that point, and
`scanKeys()` read the outdated values while `touchRead()` read the updated
values.

In BlocksDS, `scanKeys()` is used to latch the current state of the keys and the
touch screen. This forces the developer to call `scanKeys()`, but it also
ensures that there are no race conditions, as `scanKeys()` will read all the
state atomically.

## 6. Updating legacy devkitARM homebrew

Occasionally, various components of the devkitARM toolchain introduced breaking
changes of their own, which lead to the unfortunate situation of homebrew being
stuck on legacy, buggier and less reliable versions of toolchains. These issues
also need to be resolved before updating to BlocksDS. Some known issues and their
remedies are documented here; note that this list is not exhaustive.

### New assembly function definition syntax

For ARM/Thumb interwork (calling ARM functions from Thumb and vice versa) to work
correctly in more recent versions of the GNU toolchain, functions must be
appropriately marked in the generated object file. While they are annotated 
automatically for code generated by GCC, it has to be done manually for assembly
code. For example, a snippet of assembly as follows:

```asm
   .global myFunction
myFunction:
```

should be updated to read as follows:

```asm
   .global myFunction
   .type myFunction, %function
myFunction:
```

The important part is the addition of the `.type` annotation.

### Legacy register names

Old versions of libnds used alternate name defines for the DS console's memory
mapped registers. They are primarily distinguished by not having the `REG_`
prefix; for example, `SUB_BLEND_CR` has been replaced by `REG_BLDCNT_SUB`
in newer versions.

A translation table is provided in `#include <nds/registers_alt.h>`; this can
be used as a stopgap to compile the project and update all uses of legacy
register names.

### Other changes

* `DIR_ITER*`, `diropen`, `dirnext` and `dirclose` are non-standard libfat
  extensions; they should be replaced by `DIR*`, `opendir`, `readdir` and
  `closedir`, respectively.
