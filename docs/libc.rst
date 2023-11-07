#######################
libc port documentation
#######################

Introduction
============

BlocksDS has code to allow libc functions to work on NDS programs. The supported
functions are described in this file. Note that they should only be used from
ARM9 code, as they aren't supported from the ARM7.

argc and argv
=============

For NDS and NDS Lite consoles that run ROMs from a flashcard, the loader needs
to support the argv protocol of libnds. If so, ``argv[0]`` will hold the path of
the NDS file in the SD card of the flashcard. The drive name is ``fat:``. For
example, a valid path is ``fat:/path/to/rom.nds``.

For DSi consoles loading ROMs from the internal SD card, ``argv[0]`` works in a
similar way. It will hold the path to the NDS ROM in the internal SD card, which
uses ``sd:`` as prefix. For example, ``sd:/path/to/rom.nds``.

Note that it is possible to use a NDS flashcard on DSi. If the user loads the
NDS ROM from a flashcard, ``argv[0]`` will point to the filesystem of the
flashcard (``fat:``), not the internal SD card (``sd:``).

Other ``argv`` entries may be set as well, but this isn't a common occurrence.

For more information, check: https://devkitpro.org/wiki/Homebrew_Menu

Filesystem
==========

Functions like ``fopen``, ``opendir``, etc. For more information, check the
`filesystem documentation <filesystem.rst>`_.

NitroFS
=======

NitroFS (``nitro:``) is supported, but the implementation is different than the
one in ``libfilesystem``. It should be 100% compatible with it. Please, report
any bugs you may find with it.

Text console (stdout, stderr, stdin)
====================================

``stdout`` and ``stderr`` can be redirected to the libnds console or to the
no$gba debug console. They are unbuffered. When text is sent to them (by
using ``printf()`` or ``fprintf(stderr, ...)`` it's always sent to the console.
The exceptions are ANSI escape sequences, which are buffered until the end of
the sequence is received, and then sent to the console low level functions.

Remember that you can use ``fflush(stdout)`` or ``fflush(stderr)`` if buffering
becomes an issue.

For more information about ANSI escape sequences, check the following link:
https://en.wikipedia.org/wiki/ANSI_escape_code

``stdin`` is tied to the keyboard of libnds. When ``sscanf(stdin, ...)`` is
called, for example, the keyboard of libnds is used as input device. Please,
check the examples to see how to use it.

Memory allocation
=================

Functions like ``malloc``, ``calloc``, ``memalign`` and ``free`` work as usual.
They always return space from the main RAM region.

Time
====

``gettimeofday`` and ``time`` are supported. Note that, at the time of writing
this document, they only work in real hardware and no$gba. Other emulators show
the time frozen in time because they don't emulate the RTC interrupt of the
ARM7.

Exit
====

If the NDS ROM loader supports it, ``exit`` can be used to return to the loader.

For more information, check this `document <./exit-to-loader.rst>`_.
