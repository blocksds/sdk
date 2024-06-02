---
title: 'Filesystem support'
weight: 10
---

## 1. Introduction

BlocksDS supports using standard C functions to access the filesystem. It is
possible to use them to access both the SD cards of flashcarts, and the internal
SD slot of the DSi. This code is integrated in `libnds`, so you don't need to
do anything special to use it.

Add the following to your project:

```c
#include <fat.h>

int main(int argc, char *argv[])
{
    bool init_ok = fatInitDefault();
    if (!init_ok)
    {
        // Handle error
    }

    // Rest of the code
}
```

This function whether it's running on a regular DS or a DSi.

- DS: It will try to use DLDI to initialize access to the SD card of the
  flashcart. If it isn't possible it returns false. If it succeedes, it returns
  true.

- DSi: It will try to initialize access to the internal SD slot, and the SD of
  the flashcart. It will only return false if the internal slot of the DSi can't
  be accessed, and it will return true if it can.

The initial working directory is "fat:/" on the DS (DLDI), and "sd:/" on DSi.
On the DSi it is possible to switch between both filesystems with `chdir()`.

It is also possible to embed a filesystem in the NDS ROM with NitroFS. This is
a good way to keep all the assets and code of your game as one single file. You
can access files in this filesystem by using the drive name `nitro:/` in
any path provided to the C library functions.

```c
#include <filesystem.h>

int main(int argc, char *argv[])
{
    // Call fatInitDefault() here if you want.

    bool init_ok = nitroFSInit(NULL);
    if (!init_ok)
    {
        // Handle error. You probably want to hang here if NitroFS can´t be
        // read, because all of your assets will be unavailable. Another
        // option is to return from main() or call exit() to return to the
        // loader.
    }

    // Rest of the code
}
```

Check [this example](https://github.com/blocksds/sdk/tree/master/examples/filesystem/nitrofs)
to see how to use `NitroFS`.

## 2. Supported functions

You may use functions such as `fopen`, `fread`, `fwrite`, `fseek`,
`fclose`, `stat`, `rename`, `truncate`, `mkdir`, `unlink`,
`access`, `chdir`, `getcwd`. `fstat` works, but a limitation: it doesn't
have access to the modification date of the file, while `stat` does.

You can also use `open`, `read`, `write`, `lseek` and `close`, but
this isn't as common.

The `dirent.h` functions to read the contents of directories are available:
`opendir`, `closedir`, `readdir`, `rewinddir`, `seekdir`, `telldir`.

It's easy to find information online about how to use all of them. If any
specific function isn't supported, raise an issue to request it.

## 3. Running on hardware

If your flashcart doesn't do it automatically, patch your ROM with the DLDI
patcher. Most flashcarts do this automatically.

## 4. Running on emulators

Filesystem access works in several emulators. The following ones have been
tested:

- melonDS: The ROM runs in DS/DSi mode.
- no$gba: The ROM runs in DSi mode. A FAT image needs to be used.
- DeSmuMe: The ROM needs to be DLDI patched, it only runs in DS mode.

### melonDS

melonDS supports both DLDI in DS/DSi modes, and the internal SD in DSi mode.

It supports using a folder as a base for the SD of the DSi, or for a DLDI device
for DS.

Open "Emu settings". The "DSi mode" and "DLDI" tabs let you select the folders
to use as root of the filesystems (or the filesystem images to be used, if you
prefer that).

### no$gba

no$gba supports DSi mode. You must generate a FAT filesystem image with
`tools/imgbuild`. The sample `Makefile` of the provided templates have a
target that lets you do this automatically. Open the `Makefile` and set the
variables `SDROOT` and `SDIMAGE`. To build the image, run:

```bash
make sdimage
```

no$gba requires that the image is called `DSi-1.sd` and is located in the same
directory as no$gba. Set `SDIMAGE` to `<path-to-folder>/DSi-1.sd` to avoid
renaming the file all the time.

Then, open no$gba as normal.

### DeSmuMe

It supports DS mode only. Run the following command when building the ROM:

```bash
make dldipatch
```

This will patch the ROM with the DLDI driver of the R4, which is required for
the emulator to access the filesystem.

You will need to set the "Slot 1" configuration to "R4", and set the directory
to the folder that will act as root of your filesystem. If using DeSmuMe with a
graphical interface, the settings can be found in "Config > Slot 1". If using it
through the command line, run it like this:

```bash
desmume --slot1=R4 --slot1-fat-dir=<path-to-folder> <path-to-rom>.nds
```
