#####################
BlocksDS design guide
#####################

1. Custom build of GCC vs not custom build
==========================================

Building GCC is a long and not-too-easy process that doesn't give you a big
advantage over the other approach. It would make setting up the SDK a more
fragile process.

However, in order to take advantage of per-CPU optimizations and to have a
robust toolchain, it is required to use a custom build of GCC. For this purpose,
BlocksDS relies on `Wonderful Toolchain <https://wonderful.asie.pl/>`_.

2. All libraries must be easy to be built by hand
=================================================

Most SDKs come with pre-built libraries and users only need to build them
manually if they want to modify them. This is supported by BlocksDS thanks to
Wonderful Toolchain.

However, part of the reason for creating this SDK is to show users how easy it
actually is to modify any part of the code in your program. The build process
takes between a few seconds to half a minute, depending on your computer. This
is a shorter time than most installation processes of other SDKs. There just
isn't that much to build!

The exceptions are ``picolibc`` (standard C library) and ``libstdc++`` (standard
C++ library), which come with the toolchain.

3. ``picolibc`` vs ``newlib``
=============================

The C library `picolibc <https://github.com/picolibc/picolibc>`_ is used in
BlocksDS instead of `newlib <https://sourceware.org/newlib/>`_. The reasons are:

- Clearer licensing: The maintainer of ``picolibc`` has ensured that all of the
  library has consistent licensing.

- Easier configuration: For example, ``picolibc`` lets you select build targets
  easily. ``newlib`` finds all the architectures supported by your ARM cross
  compiler and it builds the library once for each architecture. ``picolibc``
  lets you give it a list of desired targets, cutting the build time to a
  fraction of the build time of ``newlib``.

  It is possible to patch ``newlib`` to only build the desired architectures,
  but keeping patches outside of the main branch of the repository is a
  maintenance burden. ``picolibc`` can be used right away without any need for
  patches.

The main disadvantage I've seen is that the documentation of ``picolibc`` about
how to port it to a new system is worse than the one of ``newlib``.

4. Standard C library port
==========================

``picolibc`` only provides the generic functionality of the standard C library.
For example, it provides versions of ``memset()`` or ``strlen()`` that are
functional. However, it can't access any OS services, so functions like
``malloc()`` or ``fopen()`` don't work right away. It is needed to port them to
the platform.

``libnds`` is the library that has the drivers to access the SD card, that
provides ``argc`` and ``argv`` to ``main()``, and that knows where to locate the
heap memory used by ``malloc()``.

For example, for ``malloc()`` to work, ``picolibc`` expects the port to provide
a function called ``sbrk()``. This function needs to get information from
``libnds`` to work. The glue code between ``picolibc`` and ``libnds`` is in
``libnds``, in ``source/arm9/libc``. This library adds support to the ARM9 core
to a lot of standard C features:

- ``stdout``, ``stderr``: They use the console provided by ``libnds``, which can
  print to the screen of the DS or the debug console of ``no$gba``.

- ``stdin``: Any call that tries to read from ``stdin`` will open the keyboard
  provided by ``libnds`` (if it wasn't open already).

- Filesystem access: ``fopen()`` and related functions, ``open()`` and related
  functions, and other functions like ``stat()`` or ``opendir()`` are supported.
  Check `this document <./filesystem.rst>`_ for more information about what's
  supported.

- Memory allocation functions: ``malloc()``, ``free()``, etc are supported.

- Real time clock: ``time()`` and ``gettimeofday`` are supported. Note that most
  emulators don't emulate the RTC interrupt of the ARM7 core, only no$gba does,
  so the time will be frozen in all other emulators. It will also work on
  hardware, though.

- Exit program: If the program calls ``exit()`` or it returns from ``main()``,
  and the loader of the NDS file supports it, it will return to the loader.

- The port of libnds in this SDK contains ``ndsabi``, with optimized versions of
  ``memcpy()``, ``memset()``, and other functions.

The reason to keep this as its own library, instead of adding it to
``picolibc``, is to make updating ``picolibc`` easier. It could also be added to
``libnds``, but that means it wouldn't be possible to reuse it for other
libraries in the future.

5. Filesystem support
=====================

This section will describe how the filesystem support has been implemented in
``libnds``. Check `this document <./filesystem.rst>`_ if you're interested in
the C standard functions that are supported.

Filesystem support requires 3 things:

- Something that provides C standard library functions, like ``fopen()``. This
  is done ``picolibc``.

- Something that reads raw bytes from the SD card. This is done by ``libnds``.

- Something that understands the raw bytes read from the SD card and interprets
  it as a FAT filesystem. This is done by `Elm's FatFs library
  <http://elm-chan.org/fsw/ff/00index_e.html>`_

Also, it is needed to provide glue code between the 3 components. For example:

``picolibc`` provides ``fopen()``, and expects the user to implement ``open()``,
which should work like the Linux system call ``open``. ``open()`` must have code
that calls functions in FatFs to do the right thing. In this case, ``open()``
translates its arguments to arguments that ``f_open()`` from FatFs can
understand.

Internally, ``f_open()`` requires a function called ``disk_read()``, which calls
``libnds`` functions to read raw bytes from the SD card. Reading raw bytes is
complicated. If you're running the code on a DSi, and you want to read from the
internal SD card, you need one specific driver. If you are running the code from
a DS slot 1 flashcart, for example, the instructions of how to read from the SD
card are provided as a DLDI driver. ``f_open()`` must determine the location of
the file (based on the filesystem prefix, ``fat:`` or ``sd:``) and use DLDI
driver functions or DSi SD driver functions accordingly.

6. NitroFS
==========

When creating a game, it is needed to add a lot of assets such as graphics and
music. Initially, most people just include them in their ARM9 binary, but this
is a bad idea. ARM7 and ARM9 binaries are loaded into RAM. There are only 4 MiB
of available memory (actually, a bit less than that, some RAM is used for things
like a hook to exit to the loader). The ARM9 is loaded in full to RAM. On top of
that, you also need RAM for your program to work. This means that, in most
cases, you're limited to 1 or 2 MiB binaries. This isn't enough for big enough
projects. There is the option to provide a folder with all your assets and tell
your users to copy it to their SD card, but this is messy.

The solution is to append a filesystem to the NDS ROM. Commercial games use a
filesystem format called Nitro ROM Filesystem. This is a custom format designed
by Nintendo. There is a library that can be used to access this filesystem,
called `libfilesystem <https://github.com/devkitPro/libfilesystem>`_ (formerly
`Nitrofs <http://blea.ch/wiki/index.php/Nitrofs>`_). The problem is that this
library doesn't have an open source license. Instead of using this library,
BlocksDS has a reimplementation of NitroFS programmed by asie, which should be
fully compatible with `libfilesystem``.

Accessing the filesystem itself is tricky.

Commercial games access it by issuing card read commands that only work on
emulators and real cartridges. Flashcarts and homebrew loaders would need to
patch the instructions, which isn't viable for homebrew games. The solution is
``argv``.

When it is initialized, ``NitroFS`` checks if ``argv[0]`` has been provided and
it can be open. ``argv[0]`` is a path to the NDS ROM being run. For example, it
may look like ``fat:/games/my-game.nds`` if the game has been opened from a
flashcart.

First, ``NitroFS`` will try to open the file using ``FatFs``. If it can be
opened, whenever ``fopen()`` is called with a path that starts with ``nitro:/``,
``FatFs`` will read blocks from the file in ``argv[0]`` with ``fseek()`` and
``fread()``.

If it fails, which should be the case in most emulators (unless they are set up
in special ways), it will try to use card read commands. The commands should
work in all emulators.

This system makes it possible to use the integrated filesystem transparently.
The developer doesn't need to worry about how it is being accessed, ``NitroFs``
will handle that complexity.

7. DLDI in the ARM7
===================

Unlike other development kits, BlocksDS supports running DLDI from either the
ARM9 or the ARM7. Please, read `this document <dldi-arm7.rst>`_ for more
information.

8. Multithreading
=================

The original ``libnds`` didn't support any kind of multithreading. This made it
impossible to fully utilize the CPUs of the NDS. For example, it wasn't possible
to use the ARM7 to load files while the ARM9 is running the application. It was
required to pause the application, load files, and continue (or try to be very
careful when loading files, which was complicated to do).

BlocksDS supports cooperative multithreading. By integrating it with ``libnds``
it is possible for functions like ``fopen`` or ``fread`` to switch to a
different thread while they are waiting for the SD card to finish reading a
block.
