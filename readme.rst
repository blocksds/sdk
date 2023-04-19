############
BlocksDS SDK
############

1. Introduction
***************

This is an SDK for Nintendo DS. It builds on the work of several people, and it
has the following goals:

- To create an easy-to-use NDS software development kit based on GCC with as few
  dependencies as possible.

- To encourage developers to hack and improve the SDK by letting them have all
  the source code of the SDK. This also shows how easy it is to build the whole
  codebase. The exception is the tolchain, which is non-trivial to build.
  Because of that, `Wonderful Toolchains <https://wonderful.asie.pl/>`_ is used
  instead of asking the users to build it themselves.

- To try to keep compatibility with pre-existing C projects. In theory, it
  should be enough to replace the makefile by one of BlocksDS to build any
  project that uses ``libnds``.

This project is currently in beta stage, and most features are working. Please,
check the `libc port documentation <docs/libc.rst>`_ for more information about
the supported libc functions.

If you want to port a project built with devkitPro, follow `this guide
<docs/porting-guide.rst>`_ for instructions.

For detailed information about the design of BlocksDS, check `this other document
<docs/design-guide.rst>`_.

For licensing information, check `this <docs/licenses.rst>`_.

2. Setup
********

If you are using Linux, MinGW, WSL, or any similar environment, you can install
the SDK using ``wf-pacman``, Docker, or the native installation instructions.

If you are on Windows, you need to install WSL and follow the Linux
instructions, or to use the Docker instructions.

The Docker images of BlocksDS can be used in any OS. They require more disk
space than the native installation, but they may be needed if a native
installation isn't possible.

Option 1: ``wf-pacman``
=======================

This is the easiest option, but you may not get the latest version of the SDK
right away.

1. You will also need to install the ARM toolchain of Wonderful Toolchains.
   Follow the instructions in this link but you skip step 5:
   https://wonderful.asie.pl/doc/general/getting-started/#linux

2. Now, install the ARM toolchain of Wonderful Toolchains, as well as the
   BlocksDS packages.

   .. code:: bash

       export PATH=/opt/wonderful/bin:$PATH
       wf-pacman -Syu
       wf-pacman -S toolchain-gcc-arm-none-eabi

   For the latest stable version, run:

   .. code:: bash

       wf-pacman -S thirdparty-blocksds-toolchain

   For the latest unstable version, run:

   .. code:: bash

       wf-pacman -S thirdparty-blocksds-git-toolchain

3. Optional step. You can also install some third party libraries like
   `NightFox’s Lib <https://github.com/knightfox75/nds_nflib>`_ and
   `Nitro Engine <https://github.com/AntonioND/nitro-engine>`_ using
   ``wf-pacman``:

   .. code:: bash

       wf-pacman -S thirdparty-blocksds-nflib \
                    thirdparty-blocksds-nitroengine

Option 2: Docker
================

Follow the instructions in `this file <docker/readme.rst>`_ to use the Docker
images.

Option 3: Build the SDK yourself
================================

This is the option you should use if you want to help develop BlocksDS, or if
you want to always use the cutting edge version of BlocksDS.

1. You need to install a cross compiler to build applications for NDS. You also
   need to build a few host tools, so you need a compiler for the host and the
   FreeImage library.

   If you're on Ubuntu, for example, run the following command:

   .. code:: bash

       sudo apt-get install -y --no-install-recommends \
           build-essential libfreeimage-dev

2. You will also need to install the ARM toolchain of Wonderful Toolchains.
   Follow the instructions in this link but you skip step 5:
   https://wonderful.asie.pl/doc/general/getting-started/#linux

3. Now, install the ARM toolchain of Wonderful Toolchains:

   .. code:: bash

       export PATH=/opt/wonderful/bin:$PATH
       wf-pacman -Syu
       wf-pacman -S toolchain-gcc-arm-none-eabi

4. When you're done, remember to add the following to your ``PATH``:

   .. code:: bash

       export PATH=/opt/wonderful/toolchain/gcc-arm-none-eabi/bin/:$PATH

   You can avoid exporting ``PATH`` every time by adding it to your ``.bashrc``,
   ``.zshenv`` or similar.

5. Now, clone this repository:

   .. code:: bash

       git clone --recurse-submodules https://github.com/blocksds/sdk.git
       cd sdk

6. To build the SDK, run this from the root of this repository:

   .. code:: bash

       BLOCKSDS=$PWD make

7. Now, you have some options. The first one is recommended, the others are for
   users that may have advanced needs.

   7.1. **Recommended**. Install it in your system. You won't need to set
        ``BLOCKSDS`` or ``BLOCKSDSEXT`` manually to use the SDK. Run:

        .. code:: bash

            sudo mkdir /opt/blocksds/ && sudo chown $USER:$USER /opt/blocksds
            mkdir /opt/blocksds/external
            make install

        This will install the libraries and tools to ``/opt/blocksds/core``.
        Third party libraries and tools are expected to be installed to
        ``/opt/blocksds/external``.

        ``BLOCKDS`` defaults to ``/opt/blocksds/core`` in all Makefiles, and
        ``BLOCKDSEXT`` defaults to ``/opt/blocksds/external``.

   7.2. Install it in your system in a custom path. You will need to set
        ``BLOCKSDS`` or ``BLOCKSDSEXT`` manually to use the SDK. Run:

        .. code:: bash

            BLOCKSDS=$PWD make INSTALLDIR=my/path

        You can avoid exporting ``BLOCKSDS`` every time by adding it to your
        ``.bashrc``, ``.zshenv`` or similar.

   7.3. Use the libraries from this path. Make sure that the environment
        variable ``BLOCKSDS`` is always set to the right location when you want
        to use the SDK. The build system of the templates and examples will use
        this variable to locate the components of BlocksDS it and use them. Any
        other external library will need to be managed by you.

        You can avoid exporting ``BLOCKSDS`` every time by adding it to your
        ``.bashrc``, ``.zshenv`` or similar.

3. Test
*******

To test that everything is working, you can try building one of the templates.

There is a template that lets the programmer write the code for both CPUs of the
NDS (``templates/rom_combined``) and a template that uses the default ARM7 core
that comes with Blocks DS and lets the programmer focus on the ARM9 core. There
are also two templates to create static libraries.

For example, go to the combined template folder and run:

.. code:: bash

    cd templates/rom_combined
    make

This should have generated a NDS ROM that you can run on any emulator or
flashcart.

To do a verbose build, run ``make`` like this (this also works for the Makefile
of the SDK):

.. code:: bash

    VERBOSE=1 make

**NOTE**: The paths in the Makefile that refer to source code, includes,
graphics, data, etc, must be inside the folder of the project. That means you
can't use ``..`` in a path to go one level up from the Makefile. If you really
need to use folders outside of the folder of the project, create a symlink to
the destination, or build the other code as a static library and link it with
the project.

4. Additional libraries
***********************

This is a list of libraries that support BlocksDS and can be installed in
``/opt/blocksds/external``:

- **`NightFox’s Lib <https://github.com/knightfox75/nds_nflib>`_**

  Library designed to make development of 2D games easy, with support for
  sprites, backgrounds (even "infinite" maps), and it also has limited 2D
  graphics support using the 3D engine.

- **`Nitro Engine <https://github.com/AntonioND/nitro-engine>`_**

  Library to create 3D applications. It supports dual screen 3D, static and
  animated models, supports all texture types, and has basic 2D support using
  the 3D hardware. It's compatible with NightFox's Lib.

5. Keeping multiple versions of BlocksDS
****************************************

You may keep multiple versions of this SDK in your PC. The location of the
active SDK is stored in the environment variables ``BLOCKSDS`` and
``BLOCKSDSEXT``, so all you need to do is to change their values and point to
the version of the SDK you want to use.

By default, the paths are assumed to be ``/opt/blocksds/core/`` and
``/opt/blocksds/external/``. The first one is used for core BlocksDS libraries
and tools, and it is managed by BlocksDS. The second one is left for users to
freely install third party libraries and tools.


6. Credits
**********

This project wouldn't have been possible without:

- Many thanks to `asie <https://asie.pl>`_ for his help and advice, and for
  `Wonderful Toolchains <https://wonderful.asie.pl/>`_
- `devkitPro <https://devkitpro.org/>`_
- `picolibc <https://github.com/picolibc/picolibc>`_
- `libnds <https://github.com/devkitPro/libnds>`_
- `dswifi <http://akkit.org/dswifi/>`_
- `maxmod <https://maxmod.devkitpro.org/>`_ (and `mmutil`)
- `grit <https://www.coranac.com/projects/grit/>`_
- `GCC <https://gcc.gnu.org/>`_
- `make <https://www.gnu.org/software/make/>`_
- And many others!
