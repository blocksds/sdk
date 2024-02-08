---
title: 'Windows'
weight: -10
---

The following instructions describe how to install BlocksDS on Windows natively.
If you want to use it in WSL, follow the instructions for Linux instead.

1. You will need to install the ARM toolchain of Wonderful Toolchains. Follow
   the instructions in this link:
   https://wonderful.asie.pl/docs/getting-started/#windows-via-msys2

1. Now, install the ARM toolchain of Wonderful Toolchains, as well as the
   BlocksDS packages.

   ```bash
   wf-pacman -Syu
   wf-pacman -S toolchain-gcc-arm-none-eabi
   ```

   Install the following if you plan to develop binaries for the DSP of the DSi:

   ```bash
   wf-pacman -S toolchain-llvm-teak-llvm
   ```

   For the latest stable version of BlocksDS, run:

   ```bash
   wf-pacman -S thirdparty-blocksds-toolchain
   ```

   To install the documentation and examples you can install:

   ```bash
   wf-pacman -S thirdparty-blocksds-docs
   ```

   For the latest unstable version, install the following packages instead:

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
