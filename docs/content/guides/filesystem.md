---
title: 'Filesystem support'
weight: 10
---

## 1. Introduction

BlocksDS supports using standard C functions to access different filesystems of
the DS:

- SD card of Slot-1 or Slot-2 flashcarts (using DLDI drivers).
- Embedded files in the NDS ROM (NitroFS).

Additionally, in DSi consoles:

- Integrated SD slot.
- Internal NAND memory.

If you want to know how to initialize and access each filesystem, keep reading.

The developer needs to initialize the filesystems before using them, but then
they can be accessed using standard C functions.

You may use functions such as `fopen()`, `fread()`, `fwrite()`, `fseek()`,
`fclose()`, `stat()`, `rename()`, `truncate()`, `mkdir()`, `unlink()`,
`access()`, `chdir()`, `getcwd()`. Function `fstat()` works, but a limitation:
it doesn't have access to the modification date of the file. `stat()` has access
to that date.

You can also use `open()`, `read()`, `write()`, `lseek()` and `close()`, but
they aren't used as frequently as the ones mentioned before.

The `dirent.h` functions to read the contents of directories are available:
`opendir()`, `closedir()`, `readdir()`, `rewinddir()`, `seekdir()`, `telldir()`.

It's easy to find information online about how to use all of them. There are
[examples](https://github.com/blocksds/sdk/tree/master/examples/filesystem/) of
using filesystem functions in the main repository of BlocksDS, you can also use
them as reference.

If any specific function isn't supported, raise an issue to request it.

## 2. Slot-1 and Slot-2 flashcarts

Slot-1 and Slot-2 flashcarts usually contain a SD or microSD card where the user
stores files. If the flashcart has a [DLDI driver](https://www.chishm.com/DLDI/)
it's possible to access it from homebrew applications. You can use it to read
additional data for your program or to write saved data, for example.

Most flashcarts autopatch NDS ROMs with the right DLDI driver, but some (the
very old ones) don't do it automatically, and they force the user to do it
manually. BlocksDS comes with `dldipatch`, which you can use to patch your ROMs.
Check [this archive](https://github.com/DS-Homebrew/DLDI) and look for your
flashcart, then use `dldipatch` like this:

```sh
$BLOCKSDS/tools/dldipatch/dldipatch patch dldi_driver.dldi rom.nds
```

Note: `dlditool` is still included for backwards-compatibility with older
Makefiles and scripts, but its use is discouraged.

Once the ROM is patched, you can initialize the filesystem and use it like this:

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

    FILE *f = fopen("fat:/myfile.txt", "rb");

    // ...
}
```

## 3. NitroFS

In most cases it isn't practical to distribute a NDS ROM with a bunch of
additional files that users have to copy to their flashcart. NitroFS is a
filesystem that is appended to the end of the NDS ROM file. The files aren't
loaded to RAM when the application is loaded, they are simply appended there so
that all the data used by the application is self-contained.

The main problem of NitroFS is finding the NDS ROM so that it can read its
contents. This has to be done in very different ways depending on where it runs:

- Emulators: They support official Nintendo cartridge read commands. NitroFS can
  use the same commands to easily access the files the same way as any official
  game.

- SD cards and NAND: If the application is in a filesystem it can't be read with
  official cartridge commands. The application needs to know where the file is
  located to open it and read from it. Applications written in C can use the
  value of `argv[0]` provided to the `main()` function, and that's what NitroFS
  uses internally. However, many loaders of ROMs don't provide this value. You
  will need to use a more modern loader that supports the homebrew `argv`
  protocol, like [NDS Homebrew Menu](https://github.com/devkitPro/nds-hb-menu/releases).
  You can boot this menu from the loader of your flashcard and then load the
  homebrew application from there.

- Unlaunch: If you load your application with Unlaunch it won't use the homebrew
  `argv` protocol, but it uses a different system used by official DSi
  applications. It's equivalent, but it limits the length of the path to 64
  characters. If your application is inside folders with long names this will be
  an issue.

Important note: This filesystem is read only!

To use this filesystem you need to add some files to it. Open your `Makefile`
and look for `NITROFSDIR`. You can add a list of directories (separated by
spaces) that will be added to the filesystem. The contents of all directories
will be merged into a single filesystem.

Then, in your code, do this:

```c
#include <filesystem.h>

int main(int argc, char *argv[])
{
    // Call fatInitDefault() here if you want to also access the SD card. It
    // isn't required if the only thing you want to use is NitroFS.
    // nitroFSInit() calls fatInitDefault() internally if it's required.

    bool init_ok = nitroFSInit(NULL);
    if (!init_ok)
    {
        // Handle error. You probably want to hang here if NitroFS can´t be
        // read, because all of your assets will be unavailable. Another
        // option is to return from main() or call exit() to return to the
        // loader.
    }

    FILE *f = fopen("nitro:/myfile.txt", "rb");

    // Rest of the code

    // ...
}
```

Important note: An empty NitroFS filesystem will cause `nitroFSInit()` to fail.

Check [this example](https://github.com/blocksds/sdk/tree/master/examples/filesystem/nitrofs)
to see how to use `NitroFS`.

**Note for Slot-2 flashcarts**

If you're using NitroFS in a Slot-2 flashcart that doesn't support DLDI, it will
still work. The flashcart will load the full NDS ROM to its RAM, and libnds will
read data from the Slot-2 memory region. However, this memory area has a size of
32 MB. If your NDS ROM is bigger than 32 MB it won't be able to access all data.
This can cause unexpected errors if it isn't detected, so `nitroFSInit()` will
simply return an error if the ROM is bigger.

## 4. DSi SD card slot

The driver to use the SD card slot of the DSi is integrated in libnds, so you
don't need to do any special patching to use it. Simply call `fatInitDefault()`
as explained before. To open files from this slot, use `"sd:/"` instead of
`"fat:/"` when opening files.

## 5. Internal DSi NAND

The driver to use the internal NAND partitions is also included in libnds.
`fatInitDefault()` tries to initialize access to NAND, but it isn't a reliable
way to know if it has been initialized or not (it will return success even if
NAND hasn't been initialized). Also, it only initializes NAND in read-only mode
(so that applications stored in NAND can use NitroFS).

It's a good idea to initialize NAND as read-only unless you really really need
to write to it. Users will definitely dislike it if you use NAND to store data
casually. The DSi has a SD slot, use that instead. `nandInit(true)` initializes
it in read-only mode, `nandInit(false)` initializes it in read/write mode.

Note that there are two partitions supported by libnds. `"nand:/"` is the main
NAND partition, and `"nand2:/"` is the partition used to store photos by the
system camera application.


```c
#include <fat.h>

int main(int argc, char *argv[])
{
    bool init_ok = nandInit(true); // true = read-only, false = read/write
    if (!init_ok)
    {
        // Handle error.
    }

    // Open file from the main NAND partition
    FILE *f1 = fopen("nand:/myfile.txt", "rb");

    // Open file from the partition used to store photos
    FILE *f2 = fopen("nand2:/myfile.txt", "rb");

    // Rest of the code

    // ...
}
```

## 6. Using multiple filesystems at the same time

You can use all filesystems at once after initializing it. All you need to do is
use the right prefix. For reference:

- `"fat:/"`: Slot-1 or Slot-2 flashcart.
- `"nitro:/"`: NitroFS.
- `"sd:/"`: SD slot of the DSi.
- `"nand:/"`: Main NAND partition of the DSi.
- `"nand2:/"`: NAND partition used to store photos.

You can use absolute paths like `sd:/folder/file.txt` or you can use `chdir()`
to switch to the main filesystem you want to use and then use relative paths.
The default filesystem depends on the console type and how many filesystems you
have initialized.

`fatInitDefault()` sets the default filesystem to `"fat:/"` in DS. In DSi it sets
it to `"sd:/"` unless the ROM has been loaded from a Slot-1 cartridge and
`argv[0]` is a path inside `"fat:/"`.

In the unusual case of a program running from NAND, the default filesystem will
be `"nand:/"`.

This is annoying to keep track of, so you can use two helpers of libnds:

- `fatGetDefaultDrive()` returns a `const char *` that will contain `"fat:/` in
  a DS. In DSi it may contain `"fat:/"`, `"sd:/"` or `"nand:/"` depending on
  where your ROM is located.

  Important: This pointer must not be passed to `free()`.

- `fatGetDefaultCwd()` returns a `char *` that contains the path to the folder
  that contains the NDS ROM (excluding the name of the ROM). If your ROM is
  located in `sd:/folder/file.nds` it will return `sd:/folder/`. If `argv[0]`
  isn't provided it will return the same value as `fatGetDefaultDrive()`.

  Important: Remember to call `free()` to free the memory used by this string
  after you have used it.

For example, if most of what you do is use NitroFS, but you sometimes write
saved data to the same folder that contains your ROM, you can do:

```
int main(int argc, char *argv[])
{
    bool init_ok = fatInitDefault();
    if (!init_ok)
    {
        // Handle error and hang
    }

    init_ok = nitroFSInit();
    if (!init_ok)
    {
        // Handle error and hang
    }

    // Set NitroFS as the default filesystem
    if (chdir("nitro:/") != 0)
    {
        // Handle error and hang
    }

    FILE *f = fopen("background/data.bin", "rb");

    // Load game data

    // ...

    // When you want to save data

    char *cwd = fatGetDefaultCwd();
    char path[512];
    snprintf(path, sizeof(path), "%s/savedata.bin", cwd);
    FILE *fout = fopen(path, "wb");
    free(cwd);

    // Now you can write data to fout
}
```

## 7. Running on emulators

Filesystem access works in several emulators. The following ones have been
tested:

- melonDS: Slot-1 is emulated (it autopatches DLDI), DSi SD card is emulated.
- no$gba: DSi SD card is emulated.
- DeSmuMe: Slot-1 is emulated (the user must patch it).

### melonDS

melonDS supports both DLDI in DS/DSi modes, and the internal SD in DSi mode.

It supports using a folder as a base for the SD of the DSi, or for a DLDI device
for DS.

Open "Emu settings". The "DSi mode" and "DLDI" tabs let you select the folders
to use as root of the filesystems (or the filesystem images to be used, if you
prefer that).

You don't need to patch the ROM with any special DLDI to use the Slot-1
emulation, melonDS does it automatically when you load the ROM.

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
