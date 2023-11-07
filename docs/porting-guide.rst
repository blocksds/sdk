##########################################
Porting NDS devkitPro projects to BlocksDS
##########################################

1. Introduccion
===============

Porting devkitPro projects to BlocksDS should be relatively easy in most cases.
BlocksDS includes most of the DS libraries that come with devkitPro. The biggest
difference is the build system. You will need to replace the Makefile. Another
smaller difference is that ``libfat`` and ``libfilesystem`` aren't included in
BlocksDS, and a subtle change in the text console. For more information, keep
reading.

2. Filesystem libraries
=======================

While devkitPro uses ``libfat`` and ``libfilesystem``, BlocksDS can't. They use
the ``devoptab`` interface implemented in the ``newlib`` fork of devkitPro.
BlocksDS uses ``picolibc`` instead of ``newlib``, and it implements filesystem
functions in a different way.

BlocksDS uses `Elm's FatFs library <http://elm-chan.org/fsw/ff/00index_e.html>`_
instead. This library has been ported to use the DLDI and DSi internal SD card
drivers that are provided by ``libnds``.

In the makefile, you have to remove ``-lfat`` and ``-lfilesystem``, as they
don't exist in BlocksDS.

From the point of view of the source code, you can use the same includes as when
using ``libfat`` and ``libfilesystem``:

.. code:: c

    #include <fat.h>
    #include <filesystem.h>

They provide ``fatInitDefault()`` and ``nitroFSInit()``. They should be
compatible with the ones in the original libraries. Please, report any behaviour
that isn't the same. If you need any other fuction, report it as well.

3. Build system differences
===========================

This is the biggest difference between devkitPro and BlocksDS. devkitPro
Makefiles are very complicated: they call themselves recursively from the build
directory. They also hide a lot of information from the user: they include
sub-makefiles in the devkitPro system directory, which has a lot of important
rules.

The Makefiles of BlocksDS include all available rules so that it's easy to
create a new build system based on them (for example, with CMake or Meson). They
don't use self-recursion, so they are easier to understand.

As a BlocksDS user you need to edit a few paths and variables, same as with
devkitPro. Open the Makefile of your devkitPro project and check this part (some
variables may be missing if you're not using them):

.. code:: make

    TARGET      := $(notdir $(CURDIR))   # Name of the resulting NDS file
    SOURCES     := source source/common  # Directories with files to compile
    INCLUDES    := include               # Directories with files to #include
    GRAPHICS    := graphics              # Folder with images and .grit files
    MUSIC       := audio                 # Folder with audio files for maxmod
    DATA        := data                  # Folder with .bin files
    NITRODATA   := nitrofs               # Root of your NitroFS filesystem

Copy the Makefiles from the ``rom_arm9_only`` or ``rom_combined`` to your
project, and open it. You have to copy the values to the following part, and
leave them empty if you aren't using them:

.. code:: make

    SOURCEDIRS  := source
    INCLUDEDIRS := include
    GFXDIRS     := graphics
    BINDIRS     := data
    AUDIODIRS   := audio
    NITROFSDIR  := nitrofs

Important note: ``SOURCEDIRS`` searches all directories recursively. If you
don't like this behaviour, go to the ``SOURCES_S``, ``SOURCES_C`` and
``SOURCES_CPP`` lines and add ``-maxdepth 1`` to the ``find`` command.

Note that ``TARGET`` is not part of this group. The top of the Makefile has this
other group of variables that you can also set to your own values:

.. code:: make

    NAME          := template_arm9     # Name of the resulting NDS file

    # Banner and icon information
    GAME_TITLE    := Combined ARM7+ARM9 template
    GAME_SUBTITLE := Built with BlocksDS
    GAME_AUTHOR   := github.com/blocksds/sdk
    GAME_ICON     := icon.bmp

Once this has been adapted to your desired values, you will need to link with
the libraries used by your program.

This is how it looks like in a devkitPro project:

.. code:: make

    LIBS := -ldswifi9 -lmm9 -lnds9

    LIBDIRS := $(LIBNDS)

This would be the equivalent in a BlocksDS project:

.. code:: make

    LIBS    := -ldswifi9 -lmm9 -lnds9
    LIBDIRS := $(BLOCKSDS)/libs/dswifi \
               $(BLOCKSDS)/libs/maxmod \
               $(BLOCKSDS)/libs/libnds

You can remove the dswifi or maxmod libraries if you aren't using them.

The reason for this additional complexity with ``LIBS`` and ``LIBDIRS`` is to
allow the user as much flexibility as possible when mixing and matching
libraries.

4. Integer version of ``stdio.h`` functions
===========================================

Functions like ``iprintf()`` or ``siscanf()``, provided by ``newlib``,  aren't
provided by ``picolibc``. Replace any calls to them by the standard names of
the functions: ``printf()``, ``sscanf()``, etc.

By default, the build of ``picolibc`` of BlocksDS makes ``printf()``,
``sscanf()`` and similar functions floats and doubles. This is done to increase
compatibility with any pre-existing code, but it increases the size of the final
binaries.

It is possible to switch to integer-only versions of the functions, and save
that additional space, by adding the following line to the ``LDFLAGS`` of your
Makefile:

.. code:: make

    LDFLAGS := [all other options go here] \
        -Wl,--defsym=vfprintf=__i_vfprintf -Wl,--defsym=vfscanf=__i_vfscanf

For more information: https://github.com/picolibc/picolibc/blob/main/doc/printf.md

5. Note about ``readdir()``
===========================

The expected behaviour of FatFs is to not include ``.`` and ``..`` as entries
when iterating over the entries of a directory. This is different than under
devkitARM, and it's a behaviour that may be added in the future.

Also, ``readdir()`` returns a ``struct dirent`` pointer with the field
``d_type``. This field can be used to determine if an entry is a directory or a
file. I've seen that some programs use it like this:

.. code:: c

    struct dirent *cur = readdir(dirp);
    if (cur->d_type & DT_DIR)
        printf("This is a directory\n");
    else if (cur->d_type & DT_REG)
        printf("This is a file\n");

However, this is incorrect. The right way to compare it is by checking if the
value is equal:

.. code:: c

    struct dirent *cur = readdir(dirp);
    if (cur->d_type == DT_DIR)
        printf("This is a directory\n");
    else if (cur->d_type == DT_REG)
        printf("This is a file\n");
