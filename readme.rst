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
  codebase.

- To try to keep compatibility with pre-existing C projects. In theory, it
  should be enough to replace the makefile by one of BlocksDS to build any
  project that uses ``libnds``.

This project is currently in beta stage, and most features are working. Please,
check the `libc port documentation <docs/libc.rst>`_ for more information about
the supported libc functions.

You may keep multiple versions of this SDK in your PC. The location of the
active SDK is stored in the environment variable ``BLOCKSDS``, so all you need
to do is to change its value and point to the right version.

If you want to port a project built with devkitPro, follow `this guide
<docs/porting-guide.rst>`_ for instructions.

For detailed information about the design of BlocksDS, check `this other document
<docs/design-guide.rst>`_.

For licensing information, check `this <docs/licenses.rst>`_.

2. Setup
********

If you are using Linux, MinGW, WSL, or any similar environment, you can install
the SDK using the native installation instructions.

If you are on Windows, you need to install MinGW, WSL or similar.

However, there are Docker images of BlocksDS that can be used in any OS. They
require more disk space than the native installation, but they may be needed if
a native installation isn't possible.

Docker
======

Follow the instructions in `this file <docker/readme.rst>`_ to use the Docker
images.

Native installation
===================

You need to install a cross compiler to build applications for NDS. You also
need to build a few host tools, so you need a compiler for the host, the
FreeImage library, and some build tools (Meson and Ninja):

If you're on Ubuntu, for example, run the following command:

.. code:: bash

    sudo apt-get install -y --no-install-recommends \
        build-essential gcc-arm-none-eabi libfreeimage-dev meson ninja-build \
        dosfstools mtools

Clone this repository:

.. code:: bash

    git clone --recurse-submodules https://github.com/blocksds/sdk.git
    cd sdk

To build the SDK, run this from the root of this repository:

.. code:: bash

    export BLOCKSDS=/path/to/sdk/
    make

You can avoid exporting ``BLOCKSDS`` every time by adding it to your
``.bashrc``, ``.zshenv`` or similar.

Test
****

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

Credits
*******

This project wouldn't have been possible without:

- `devkitPro <https://devkitpro.org/>`_
- `picolibc <https://github.com/picolibc/picolibc>`_
- `libnds <https://github.com/devkitPro/libnds>`_
- `dswifi <http://akkit.org/dswifi/>`_
- `maxmod <https://maxmod.devkitpro.org/>`_ (and `mmutil`)
- `grit <https://www.coranac.com/projects/grit/>`_
- `GCC <https://gcc.gnu.org/>`_
- `make <https://www.gnu.org/software/make/>`_
- `asie <https://asie.pl>`_ for his advice.
- And many others!
