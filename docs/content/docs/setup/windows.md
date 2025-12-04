---
title: 'Windows'
weight: 1
---

The following instructions describe how to install BlocksDS on Windows natively.
If you want to use it in WSL, follow the instructions for Linux instead.

{{% steps %}}

### Install Wonderful Toolchain

Follow the instructions in this link:

https://wonderful.asie.pl/wiki/doku.php?id=getting_started:windows

### Add the BlocksDS repository to `wf-pacman`

```bash
wf-pacman -Syu wf-tools
wf-config repo enable blocksds
wf-pacman -Syu
```

### Install the ARM toolchain and BlocksDS

```bash
wf-pacman -S blocksds-toolchain
```

To install the documentation and examples you can install:

```bash
wf-pacman -S blocksds-docs
```

### Install third-party libraries (Optional)

You can also install some third party libraries like
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

{{% /steps %}}

### How to update the SDK

When you want to update the packages, run:

```bash
wf-pacman -Syu
```

### How to build projects

Run **Wonderful Toolchain Shell** from the Start menu. This shell has predefined
environment variables to ensure that BlocksDS projects can be built. If you
don't, the environment variable `BLOCKSDS` won't be set to the right path.

Now you can try to build one of the BlocksDS examples. From the shell, run:

```bash
cd /opt/wonderful/thirdparty/blocksds/core/examples/
```

From there, for example, go to `graphics_3d/volumetric_shadow` and type
`make`. This will generate a NDS ROM that you can try with an emulator or a
flashcart.

You can also try one of the open source projects made with BlocksDS that are
listed [here](https://github.com/blocksds/awesome-blocksds).

> [!WARNING]
> If you get an error like `command not found: make`, you need to install it
> from the packages of your system. For example, try ``pacman -S make``
