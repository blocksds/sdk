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
instead. This library has been ported to use the DLDI and DSi interna SD card
drivers that are provided by ``libnds``.

From the point of view of the source code, you will need to replace the
following includes:

.. code:: c

    #include <fat.h>
    #include <filesystem.h>

By this other include:

.. code:: c

    #include <fatfs.h>

The ``fatfs.h`` header provides ``fatInitDefault()`` and ``nitroFSInit()``. They
should be compatible with the ones in ``libfat`` and ``libfilesystem``. Please,
report any behaviour that isn't the same. If you need any other fuction, report
it as well.

3. Text console differences
===========================

The main difference is that ``stdout`` and ``stderr`` are line-buffered, unlike
in ``libnds``. In most cases this won't be noticeable, but it's possible that in
a few situations a bit of text is missing from the screen. This can be fixed by
using the following functions:

.. code:: c

    #include <stdio.h>

    fflush(stdout);
    fflush(stderr);

The reason for this change is that ``picolibc`` breaks ANSI escape sequences if
they aren't buffered, and this would break compatibility with old ``libnds``
projects.

4. Build system differences
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

    SOURCEDIRS  := source source/common
    INCLUDEDIRS := include
    GFXDIRS     := graphics
    BINDIRS     := data
    AUDIODIRS   := audio
    NITROFATDIR := nitrofs

Note that ``TARGET`` is not part of this group. The top of the Makefile has this
other group of variables that you can also set to your own values:

.. code:: make

    NAME            := template_arm9     # Name of the resulting NDS file

    # Banner and icon information
    GAME_TITLE      := Combined ARM7+ARM9 template
    GAME_SUBTITLE1  := Built with BlocksDS
    GAME_SUBTITLE2  := http://skylyrac.net
    GAME_ICON       := icon.bmp

Once this has been adapted to your desired values, you will need to link with
the libraries used by your program.

This is how it looks like in a devkitPro project:

.. code:: make

    LIBS := -ldswifi9 -lmm9 -lnds9

    LIBDIRS := $(LIBNDS)

This would be the equivalent in a BlocksDS project:

.. code:: make

    LIBS    := -ldswifi9 -lmm9 -lsysnds9 -lnds9 -lc
    LIBDIRS := $(BLOCKSDS)/libs/dswifi \
               $(BLOCKSDS)/libs/maxmod \
               $(BLOCKSDS)/libs/libsysnds \
               $(BLOCKSDS)/libs/libnds \
               $(BLOCKSDS)/libs/libc

It is very important to keep the last 3 in that order in the ``LIBS`` variable
(``-lsysnds9 -lnds9 -lc``) and the ``LIBDIRS`` variable
(``$(BLOCKSDS)/libs/libsysnds $(BLOCKSDS)/libs/libnds $(BLOCKSDS)/libs/libc``).

You can remove the dswifi or maxmod libraries if you aren't using them.

The reason for this additional complexity with ``LIBS`` and ``LIBDIRS`` is to
allow the user as much flexibility as possible when mixing and matching
libraries. Right now, ``libsysnds``, ``libc`` and ``libnds`` are tied together,
but that may not always be the case in the future.
