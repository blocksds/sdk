---
title: NitroFS
weight: 50
---

## 1. Introduction

When you start developing a NDS application it's pretty normal to add all your
graphics and audio files as data like in all the previous chapters of this
tutorial. For example, in the chapter about backgrounds we have seen an example
in which `photo.png` and `photo.grit` are added to the `graphics` folder and
they get converted by the Makefile into `photo.c` and `photo.h` so that we can
use it this way:

```c
#include <nds.h>

#include "photo.h"

int main(int argc, char *argv[])
{
    videoSetMode(MODE_5_2D);
    vramSetBankA(VRAM_A_MAIN_BG);

    int bg = bgInit(2, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
    dmaCopy(photoBitmap, bgGetGfxPtr(bg), photoBitmapLen);

    while (1)
        swiWaitForVBlank();
}
```

This sytem is very easy to use, but it has a very strong limitation: it relies
on your code and data fitting in main RAM. The DS and DS Lite have about 4 MiB
of RAM available for the developer. The DSi has about 16 MiB. This space needs
to be shared by all your static code and data, but it's also used by your
application. Whenever you use `malloc()`, the memory it returns comes from main
RAM. If your graphics and code grow too much it's very likely that you'll reach
a point in which you don't have enough RAM for your application to run. You may
also get to the point in which your application doesn't even fit in RAM.

In practice, this means that the size of your ARM9 binary (between code and
data) can't be much bigger than 1 or 2 MiB. However, all official released NDS
games are bigger than this, so how do they do that? The answer is: They store
their data as files in a filesystem that is part of the NDS ROM.

A NDS ROM is divided into:

- ROM header: It has information about the game (title, game icon...) and about
  the address of all other parts of the ROM.

- ARM9 and ARM7 binaries: They are the static code and data that is loaded
  directly into RAM by the NDS ROM loader. This data is always present in RAM
  and you can access it whenever you want.

- Filesystem image: This is what we call NitroFS. It's just a **read-only**
  filesystem image with all the graphics, audio and data that you can't fit in
  your ARM9 and ARM7 binaries. You need to load files into main RAM when you
  want to use them.

In practice, you'll need some graphics in your ARM9 binary. NitroFS can fail to
start, and you will need to show some message to users about the reason for the
error. The easiest thing you can do is to start the demo console of libnds and
print an error message. For example, this is a common way to initialize NitroFS:

```c
#include <filesystem.h>
#include <nds.h>

int main(int argc, char *argv[])
{
    if (!nitroFSInit(NULL))
    {
        // Initialize console
        consoleDemoInit();
        // Print string followed by the cause of the error
        perror("NitroFS init error!");

        // You can also print a link to a website with instructions on how to
        // correctly load the game, for example.

        // Wait forever in a low-power mode
        while (1)
            swiWaitForVBlank();
    }

    // Continue with your game...
}
```

You can check an example of using NitroFS to read files and list files in
directories here: [`examples/filesystem/nitrofs`](https://github.com/blocksds/sdk/tree/master/examples/filesystem/nitrofs)

This chapter focuses on how to read files, how to list contents in a directory,
and how NitroFS actually works and some common problems you may have with it.

## 2. Reading files

To access files from the filesystem you can use functions from the C (`stdio.h`,
`dirent.h`) and C++ (`iostream`). However, note that the C++ standard I/O
functions increase the code footprint of your programs a lot.

There are plenty of tutorials online to learn how to use them, so this tutorial
won't get into much detail about them.

The main thing to remember is that the drive name of NitroFS is "nitro:". For
example, the full path to a file called "test.bin" in the root of the filesystem
is "nitro:/test.bin".

Important reminder: NitroFS is a read-only filesystem: you can't write to it. If
you try to open a file in write mode it will fail to be opened.

For example, this function checks the size of a file, allocates a buffer to hold
its contents, reads the file, and returns the pointer to the buffer and the size
of the buffer:

```c
#include <stdio.h>

bool file_load(const char *path, void **buffer, size_t *size_)
{
    // Open the file in read binary mode
    FILE *f = fopen(path, "rb");
    if (f == NULL)
    {
        perror("fopen");
        return false;
    }

    // Move read cursor to the end of the file
    int ret = fseek(f, 0, SEEK_END);
    if (ret != 0)
    {
        perror("fseek");
        return false;
    }

    // Check position of the cursor (we're at the end, so this is the size)
    *size = ftell(f);
    if (*size == 0)
    {
        printf("Size is 0!");
        fclose(f);
        return false;
    }

    // Move cursor to the start of the file again
    rewind(f);

    // Allocate buffer to hold data
    *buffer = malloc(*size);
    if (*buffer == NULL)
    {
        printf("Not enought memory to load %s!", path);
        fclose(f);
        return false;
    }

    // Read all data into the buffer
    if (fread(*buffer, *size, 1, f) != 1)
    {
        perror("fread");
        fclose(f);
        free(*buffer);
        return false;
    }

    // Close file
    res = fclose(f);
    if (res != 0)
    {
        perror("fclose");
        free(*buffer);
        return false;
    }

    return true;
}

int main(int argc, char *argv[])
{
    // ...

    void *buf;
    size_t size;
    if (!file_load("nitro:/test.bin", &buf, &size))
    {
        printf("Error opening file!");
    }
    else
    {
        printf("File loaded!");

        // Now you can use `buf` and `size`

        // ...
    }
    free(buf);

    // ...
}
```

**Important note:** Remember to flush the buffer after reading a file if you're
planning to use DMA to copy it somewhere! `DC_FlushRange(buffer, size)` should
be enough. libnds functions like `glTexImage2D()` do this internally, but
`dmaCopy()` expects you to have done it beforehand.

## 3. Navigating directories

You can use `chdir()` to set the base directory that you are using. For example,
you can use this to use files from folder "myfolder" without specifying the base
path every time. Instead of doing this:

```c
fopen("nitro:/myfolder/test.bin", "rb");
```

You can do this:

```c
chdir("nitro:/myfolder");
fopen("test.bin", "rb");
```

One final comment is that normally you don't need to use the "nitro:" drive name
every time you open a file. If you haven't used `chdir()` the following three
functions are equivalent:

```c
fopen("nitro:/myfolder/test.bin", "rb");
fopen("/myfolder/test.bin", "rb");
fopen("myfolder/test.bin", "rb");
```

In a future chapter you'll learn how to use other filesystems. At that point you
will need to remember using the drive name "nitro:" again.

You can check what directory you're in by running this, for example:

```c
char *cwd = getcwd(NULL, 0);
printf("Current directory: %s\n", cwd);
free(cwd);
```

## 4. Listing files in directories

A very common operation is to list the contents of a folder:

```c
#include <dirent.h>

bool directory_list(const char *path)
{
    // Open directory iterator
    DIR *dirp = opendir(path);
    if (dirp == NULL)
    {
        perror("opendir");
        return false;
    }

    while (1)
    {
        // Read next entry in the directory
        struct dirent *cur = readdir(dirp);
        if (cur == NULL)
            break;

        // "d_name" holds the name of the entry
        if (strlen(cur->d_name) == 0)
            break;

        // Get the index associated to this entry
        int index = telldir(dirp);

        // Print information of this entry. "d_type" is set to DT_DIR if the
        // entry is a directory or to DT_REG for regular files.
        printf("%d - %s%s\n", index, cur->d_name,
               (cur->d_type == DT_DIR) ? "/" : " ");
    }

    // You can use rewinddir(dirp) to return to the start of the directory

    // Close directory when you're done
    if (closedir(dirp) != 0)
    {
        perror("closedir");
        return false;
    }

    return true;
}

int main(int argc, char *argv[])
{
    // List contents of the root of the filesystem
    directory_list("nitro:/");

    // List contents of directory "testdir"
    directory_list("nitro:/testdir");
}
```

Note: Don't do `cur->d_type & DT_DIR` to determine if the file is a directory!
This variable isn't a bit field, but an enumeration. You need to check if the
value of `d_type` matches the type with `cur->d_type == DT_DIR`.

## 5. How does NitroFS work?

NitroFS is different from commercial games. While the filesystem format is the
same, the way to access it is completely different, and flashcards treat them
differently than homebrew applications.

Official games are distributed as a cartridge that contains only the NDS ROM
(plus additional storage for saved data). The cartridges have a specific
communication protocol in which the game asks the cartridge to read a specific
address from the ROM, and the cartridge returns the data. This process always
takes the same amount of time, so it's very predictable.

Homebrew games stored in a flashcard (or in the SD card of DSi consoles) are
just files in the filesystem of a SD card. The game can't just ask for an
address to read: It needs to consider the whole filesystem of the SD card. Once
the game has located the ROM file inside the SD card the game can start reading
from the desired address from the ROM.

As a side note: Official games have the same problem when running on a
flashcard. The difference is that flashcards do special handling of official
games, but they don't do anything for homebrew games.

Funnily enough, homebrew games running on an emulator behave just like official
games! This means that a homebrew game needs to be able to support both systems
to access the filesystem.

NitroFS gives a unified way to access the filesystem of your ROM regardless of
what actually needs to happen for the data to be read.

NitroFS requires two things:

- Your NDS ROM needs to be DLDI patched with the driver of your flashcard. DLDI
  is a system that lets loaders patch homebrew games with the right driver to
  read and write from the SD card of your flashcard.

- Your loader needs to provide `argv[0]` to the program. This is the location of
  the ROM inside the SD card, which is used by NitroFS to open the ROM and start
  reading from it.

Almost all loaders do DLDI patching, but there aren't many flashcards with
loaders that provide `argv[0]`. This brings up to the next section in this
chapter.

## 6. Potential issues

There are some reasons why NitroFS may fail to start:

- The loader hasn't patched the ROM with the right DLDI driver when loading it.
  This is pretty unusual, but it may happen in very old Slot-2 flashcards. DLDI
  as a standard has existed [since 2006](https://www.chishm.com/DLDI/), so it is
  almost universally supported.

- The loader hasn't provided `argv[0]` when loading the ROM. The `argv` protocol
  has existed [since 2010](https://devkitpro.org/wiki/Homebrew_Menu), so it is
  far less supported than DLDI. It's pretty common to find loaders that don't
  support it.

If you have issues with NitroFS, the best advice you can give your users is to
install a loader that supports `argv` (it will also support DLDI). My advice is
to point your users to The Nintendo DS Homebrew Menu. You can download it from
[here](https://github.com/devkitPro/nds-hb-menu/releases). The zip archive comes
with several files, but all you really need is the `BOOT.NDS` file. You can
rename it to `hbmenu.nds` or something similar. If your loader doesn't patch
DLDI automatically, patch this ROM. Then, copy it to your SD card, and load it
from your loader. This will open another menu to select the homebrew application
you want to launch, and it will DLDI patch it as well as provide `argv` to it.

## 7. Using Maxmod with NitroFS

In a previous chapter we have seen how to use Maxmod to play music and sound
effects. This was done by loading a soundbank present in the ARM9 as data. It's
possible to tell Maxmod to use a soundbank from NitroFS instead very easily.
Only the initialization of the library needs to change. Find this line:

```c
mmInitDefaultMem((mm_addr)soundbank_bin);
```

And replace it by this:

```c
mmInitDefault("nitro:/soundbank.bin");
```

Remember to remove this as well:

```c
#include "soundbank_bin.h"
```

If you're using the default Makefile of BlocksDS ARM9 projects you can make the
build system save the soundbank in NitroFS by setting `NITROFSDIR` to something.
As soon as the Makefile detects that you're using NitroFS, it will place the
soundbank in NitroFS:

```
AUDIODIRS	:= audio
NITROFSDIR	:= nitrofs
```

Check the example here: [`examples/maxmod/nitrofs`](https://github.com/blocksds/sdk/tree/master/examples/maxmod/nitrofs)

## 8. Loading GRF files

GRF is a format that can hold Nintendo DS graphics in all formats supported by
the console. It was originally designed by Cearn, the author of **grit**. This
format had some limitations in its original form, so it has been modified
slightly in BlocksDS to be able to fully describe any kind of graphics type.

However, this means that GRF files of BlocksDS and other versions of grit aren't
compatible. They use different header identification strings so that loaders of
this format can detect if they are using an incompatible file.

In order to help developers, libnds has some definitions related to the GRF
format. It also has some helper functions to load files. They can all be found
in [this header](https://github.com/blocksds/libnds/blob/master/include/nds/arm9/grf.h).

Essentially, GRF files are RIFF files that contain multiple chunks of data
(palettes, tile maps, textures, etc). The header of the file specify the format
and dimensions of the image data.

This tutorial won't get into a lot of detail on how to load the files. However,
BlocksDS has a few examples of how to use GRF files for multiple purposes. Note
that they don't use Makefiles as build system, they use ArchitectDS because it
supports creating GRF files and adding them to NitroFS using a PNG file as
input. The Makefiles don't currently support this. Check the files `build.py` to
see how they are configured to convert graphics into GRF files.

- 2D sprites:

  - [`examples/graphics_2d/sprites_grf_types`](https://github.com/blocksds/sdk/tree/master/examples/graphics_2d/sprites_grf_types)
  - [`examples/graphics_2d/sprites_nitrofs`](https://github.com/blocksds/sdk/tree/master/examples/graphics_2d/sprites_nitrofs)

- 2D backgrounds:

  - [`examples/graphics_2d/bg_regular_nitrofs`](https://github.com/blocksds/sdk/tree/master/examples/graphics_2d/bg_regular_nitrofs)

- 3D textures:

  - [`examples/graphics_3d/basic_texture_nitrofs`](https://github.com/blocksds/sdk/tree/master/examples/graphics_3d/basic_texture_nitrofs)
  - [`examples/graphics_3d/paletted_textures_nitrofs`](https://github.com/blocksds/sdk/tree/master/examples/graphics_3d/paletted_textures_nitrofs)

- There's also a demo of how to create a simple game using GRF files:

  - [`examples/demos/invaders`](https://github.com/blocksds/sdk/tree/master/examples/demos/invaders)

## 9. Using GL2D with NitroFS

In the chapter about GL2D we learned how to merge a lot of images into a single
big image plus a list of coordinates and sizes. The tool in charge of doing this
is **squeezer**. However, in that chapter we learned how to generate a pair of
C/H files. This isn't very useful if you want to keep your data in the
filesystem and only load it when it's needed.

Squeezer has a way to generate files with extension GUV (which stands for GL2D
UVs): Instead of using `--outputH` and `--outputC`, you can use `--outputNitro`.

GUV files have a small header:

Field             | Size       | Notes
------------------|------------|----------------------------------
Magic             | `uint16_t` | Equal to `0x012D`
Texture width     | `uint16_t` | In pixels.
Texture height    | `uint16_t` | In pixels.
Number of frames  | `uint16_t` | Number of sub-images in the file.

Followed by an array of frames:

Field   | Size
--------|-----------
X       | `uint16_t`
Y       | `uint16_t`
Width   | `uint16_t`
Height  | `uint16_t`

Once you load the file, the array of frames works the same way as the arrays in
the C/H files generated with `--outputH` and `--outputC`. Check the following
example: [`examples/gl2d/spriteset_nitrofs`](https://github.com/blocksds/sdk/tree/master/examples/gl2d/spriteset_nitrofs)

Note that this format doesn't contain the image itself, only the information
about the sub-images. To store the actual texture you need to use, for example,
a GRF file.
