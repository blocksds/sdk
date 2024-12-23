---
title: 'Linux'
weight: 0
---

This is the easiest option, but you may not get the latest version of the SDK
right away.

1. You will also need to install the ARM toolchain of Wonderful Toolchains.
   Follow the instructions in this link:
   https://wonderful.asie.pl/docs/getting-started/#linux

1. Add the BlocksDS repository to `wf-pacman`:

   ```bash
   wf-pacman -Sy wf-tools
   wf-config repo enable blocksds
   ```

1. Now, install the ARM toolchain of Wonderful Toolchains, as well as the
   BlocksDS packages.

   ```bash
   export PATH=/opt/wonderful/bin:$PATH
   wf-pacman -Syu
   wf-pacman -S toolchain-gcc-arm-none-eabi
   ```

   Install the following if you plan to develop binaries for the DSP of the DSi:

   ```bash
   wf-pacman -S toolchain-llvm-teak-llvm
   ```

   Now, install BlocksDS itself:

   ```bash
   wf-pacman -S blocksds-toolchain
   ```

   To install the documentation and examples you can install:

   ```bash
   wf-pacman -S blocksds-docs
   ```

1. (Optional step) You can also install some third party libraries like
   [NightFox’s Lib](https://github.com/knightfox75/nds_nflib) and
   [Nitro Engine](https://github.com/AntonioND/nitro-engine) using
   `wf-pacman`:

   ```bash
   wf-pacman -S blocksds-nflib blocksds-nitroengine
   ```

   You can get a full list of packages by running:

   ```bash
   wf-pacman -Sl
   ```

1. `wf-pacman` installs BlocksDS in `/opt/wonderful/thirdparty/blocksds/`.
   This isn't the standard location (`wf-pacman` can only write files to
   `/opt/wonderful/`) so you need to do one of the two following options:

   1. Create a symlink:

      ```bash
      ln -s /opt/wonderful/thirdparty/blocksds /opt/blocksds
      ```

   1. Set your environment variables correctly:

      ```bash
      export BLOCKSDS=/opt/wonderful/thirdparty/blocksds/core
      export BLOCKSDSEXT=/opt/wonderful/thirdparty/blocksds/external
      ```

1. Whenever you need to update the SDK, run:

   ```bash
   wf-pacman -Syu
   ```

1. Now you can try to build one of the BlocksDS examples. They are available at
   path `/opt/wonderful/thirdparty/blocksds/core/examples/`. From there, for
   example, go to `graphics_3d/volumetric_shadow` and type `make`. This will
   generate a NDS ROM that you can try with an emulator or a flashcart.

   You can also try one of the open source projects made with BlocksDS that are
   listed [here](https://github.com/blocksds/awesome-blocksds).
