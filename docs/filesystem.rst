##############################
Filesystem support in BlocksDS
##############################

Introduction
============

BlocksDS supports using standard C functions to access the filesystem. It is
possible to use them to access both the SD cards of flashcards, and the internal
SD slot of the DSi. This code is integrated in ``libnds``, so you don't need to
do anything special to use it.

Add the following to your project:

.. code:: c

    #include <fatfs.h>

    int main(int argc, char *argv[])
    {
        bool init_ok = fatInitDefault();
        if (!init_ok)
        {
            // Handle error
        }

        // Rest of the code
    }

This function whether it's running on a regular DS or a DSi.

- DS: It will try to use DLDI to initialize access to the SD card of the
  flashcard. If it isn't possible it returns false. If it succeedes, it returns
  true.

- DSi: It will try to initialize access to the internal SD slot, and the SD of
  the flashcard. It will only return false if the internal slot of the DSi can't
  be accessed, and it will return true if it can.

The initial working directory is "fat:/" on the DS (DLDI), and "sd:/" on DSi.
On the DSi it is possible to switch between both filesystems with ``chdir()``.

It is also possible to embed a filesystem in the NDS ROM with NitroFAT. This is
a good way to keep all the assets and code of your game as one single file. You
can access files in this filesystem by using the drive name ``nitro:/`` in
any path provided to the C library functions.

.. code:: c

    #include <fatfs.h>

    int main(int argc, char *argv[])
    {
        // Call fatInitDefault() here if you want.

        bool init_ok = nitroFSInit(NULL);
        if (!init_ok)
        {
            // Handle error. You probably want to hang here if NitroFAT can´t be
            // read, because all of your assets will be unavailable. Another
            // option is to return from main() or call exit() to return to the
            // loader.
        }

        // Rest of the code
    }

It is possible to call ``nitroFSInit()`` without ``fatInitDefault()``, but it
won't work on flashcarts, just emulators. Check `this example
<../examples/filesystem/nitrofat>`_ to see how to use ``NitroFAT``.

Supported functions
===================

You may use functions such as ``fopen``, ``fread``, ``fwrite``, ``fseek``,
``fclose``, ``stat``, ``rename``, ``truncate``, ``mkdir``, ``unlink``,
``access``, ``chdir``, ``getcwd``. ``fstat`` works, but a limitation: it doesn't
have access to the modification date of the file, while ``stat`` does.

You can also use ``open``, ``read``, ``write``, ``lseek`` and ``close``, but
this isn't as common.

The ``dirent.h`` functions to read the contents of directories are available:
``opendir``, ``closedir``, ``readdir``, ``rewinddir``, ``seekdir``, ``telldir``.

It's easy to find information online about how to use all of them. If any
specific function isn't supported, raise an issue to request it.

Running on hardware
===================

If your flashcard doesn't do it automatically, patch your ROM with the DLDI
patcher. Most flashcards do this automatically.

Running on emulators
====================

Filesystem access works in several emulators. The following ones have been
tested:

- DeSmuMe: The ROM needs to be DLDI patched, it only runs in DS mode.
- no$gba: The ROM runs in DSi mode. A FAT image needs to be used.
- melonDS: The ROM runs in DSi mode. A FAT image needs to be used.

1. DeSmuMe
----------

It supports DS mode only. Run the following command when building the ROM:

.. code:: bash

    make dldipatch

This will patch the ROM with the DLDI driver of the R4, which is required for
the emulator to access the filesystem.

You will need to set the "Slot 1" configuration to "R4", and set the directory
to the folder that will act as root of your filesystem. If using DeSmuMe with a
graphical interface, the settings can be found in "Config > Slot 1". If using it
through the command line, run it like this:

.. code:: bash

    desmume --slot1=R4 --slot1-fat-dir=<path-to-folder> <path-to-rom>.nds

IMPORTANT: Remember that the R4 DLDI driver doesn't have an open source license,
so don't distribute any binaries patched with this driver. Only distribute the
unpatched versions of your binaries.

2. no$gba
---------

no$gba supports DSi mode. You must generate a FAT filesystem image with
``tools/imgbuild``. The sample ``Makefile`` of the provided templates have a
target that lets you do this automatically. Open the ``Makefile`` and set the
variables ``SDROOT`` and ``SDIMAGE``. To build the image, run:

.. code:: bash

    make sdimage

no$gba requires that the image is called ``DSi-1.sd`` and is located in the same
directory as no$gba. Set ``SDIMAGE`` to ``<path-to-folder>/DSi-1.sd`` to avoid
renaming the file all the time.

Then, open no$gba as normal.

3. melonDS
----------

melonDS supports both DLDI in DS/DSi modes, and the internal SD in DSi mode.
Follow the same steps as for no$gba to generate a filesystem image, and name it
however you want. Then, open "Emu settings". The "DSi mode" and "DLDI" tabs let
you select the filesystem images to be used.
