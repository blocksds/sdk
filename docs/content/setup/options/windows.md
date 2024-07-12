---
title: 'Windows'
weight: -10
---

The following instructions describe how to install BlocksDS on Windows natively.
If you want to use it in WSL, follow the instructions for Linux instead.

1. You will need to install the ARM toolchain of Wonderful Toolchains. Follow
   the instructions in this link:
   https://wonderful.asie.pl/docs/getting-started/#windows-via-msys2

1. Install the ARM toolchain of Wonderful Toolchain:

   ```bash
   wf-pacman -Syu
   wf-pacman -S toolchain-gcc-arm-none-eabi
   ```

   Now, install BlocksDS itself. For the latest stable version of BlocksDS, run:

   ```bash
   wf-pacman -S thirdparty-blocksds-toolchain
   ```

   To install the documentation and examples you can install:

   ```bash
   wf-pacman -S thirdparty-blocksds-docs
   ```

   For the latest unstable version, install the following packages instead of
   the previous ones:

   ```bash
   wf-pacman -S thirdparty-blocksds-git-toolchain \
                thirdparty-blocksds-git-docs
   ```

1. (Optional step) You can also install some third party libraries like
   [NightFox’s Lib](https://github.com/knightfox75/nds_nflib) and
   [Nitro Engine](https://github.com/AntonioND/nitro-engine>) using
   `wf-pacman`:

   ```bash
   wf-pacman -S thirdparty-blocksds-nflib \
                thirdparty-blocksds-nitroengine
   ```

1. Whenever you need to update the SDK, run:

   ```bash
   wf-pacman -Syu
   ```

1. Run Wonderful Toolchain Shell from the Start menu. This shell has predefined
   environment variables to ensure that `BLOCKSDS` projects can be built. If you
   don't, `BLOCKSDS` won't be set to the right path.
