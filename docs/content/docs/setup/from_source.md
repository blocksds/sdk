---
title: 'Build from source'
weight: 4
---

This is the option you should use if you want to help develop BlocksDS, or if
you want to always use the cutting edge version of BlocksDS.

1. You need to install a cross compiler to build applications for NDS. You also
   need to build a few host tools, including a C/C++ compiler.

   If you're on Ubuntu run the following command:

   ```bash
   sudo apt-get install -y --no-install-recommends build-essential
   ```

1. You will also need to install the ARM toolchain of Wonderful Toolchains.
   Follow the instructions in this link:
   https://wonderful.asie.pl/doc/general/getting-started/#linux

1. Now, install the ARM toolchain of Wonderful Toolchains:

   ```bash
   export PATH=/opt/wonderful/bin:$PATH
   wf-pacman -Syu
   wf-pacman -S toolchain-gcc-arm-none-eabi
   ```

   You can also install the Teak toolchain if you want to develop binaries for
   the DSP of the DSi. This isn't available in all platforms (like in Windows).

   ```bash
   wf-pacman -S toolchain-llvm-teak-llvm
   ```

1. Now, clone this repository:

   ```bash
   git clone --recurse-submodules https://github.com/blocksds/sdk.git
   cd sdk
   ```

1. To build the SDK, run this from the root of this repository:

   ```bash
   BLOCKSDS=$PWD make -j`nproc`
   ```

1. Now, you have some options. The first one is recommended, the others are for
   users that may have advanced needs.

   1. **Recommended**. Install it in your system. You won't need to set
      `BLOCKSDS` or `BLOCKSDSEXT` manually to use the SDK. Run:

      ```bash
      sudo mkdir /opt/blocksds/ && sudo chown $USER:$USER /opt/blocksds
      mkdir /opt/blocksds/external
      make install
      ```

      This will install the libraries and tools to `/opt/blocksds/core`.
      Third party libraries and tools are expected to be installed to
      `/opt/blocksds/external`.

      `BLOCKDS` defaults to `/opt/blocksds/core` in all Makefiles, and
      `BLOCKDSEXT` defaults to `/opt/blocksds/external`.

   1. Install it in your system in a custom path. You will need to set
      `BLOCKSDS` or `BLOCKSDSEXT` manually to use the SDK. Run:

      ```bash
      BLOCKSDS=$PWD make INSTALLDIR=my/path -j`nproc`
      ```

      You can avoid exporting `BLOCKSDS` every time by adding it to your
      `.bashrc`, `.zshenv` or similar.

   1. Use the libraries from this path. Make sure that the environment
      variable `BLOCKSDS` is always set to the right location when you want
      to use the SDK. The build system of the templates and examples will use
      this variable to locate the components of BlocksDS it and use them. Any
      other external library will need to be managed by you.

      You can avoid exporting `BLOCKSDS` every time by adding it to your
      `.bashrc`, `.zshenv` or similar.

1. Now you can try to build one of the BlocksDS examples. They are available in
   folder `examples` in the root of the repository. From there, for example, go
   to `graphics_3d/volumetric_shadow` and type `make`. This will generate a NDS
   ROM that you can try with an emulator or a flashcart.

   You can also try one of the open source projects made with BlocksDS that are
   listed [here](https://github.com/blocksds/awesome-blocksds).
