---
title: 'Build from source'
weight: 5
---

This is the option you should use if you want to help develop BlocksDS, or if
you want to always use the cutting edge version of BlocksDS.

{{% steps %}}

### Install host tools

You need to install a cross compiler to build applications for NDS. You also
need to build a few host tools, including a C/C++ compiler.

If you're on Ubuntu run the following command:

```bash
sudo apt-get install -y --no-install-recommends build-essential
```

### Install Wonderful Toolchain

Follow the instructions in this link:

https://wonderful.asie.pl/wiki/doku.php?id=getting_started

### Install the ARM toolchain

Install the ARM toolchain of Wonderful Toolchain:

```bash
export PATH=/opt/wonderful/bin:$PATH
wf-pacman -Syu
wf-pacman -S toolchain-gcc-arm-none-eabi
```

### Add the BlocksDS repository to `wf-pacman`

This isn't strictly required if you're building everything from source, but you
may want to install some pre-built BlocksDS packages even if you build the rest
from source.

```bash
wf-pacman -Syu wf-tools
wf-config repo enable blocksds
wf-pacman -Syu
```

### Additional experimental toolchains

**Advanced users only!**

The following two toolchains aren't relevant for most users of BlocksDS. They
are very experimental, and they are mainly interesting for people that want to
improve them, not for people wanting to create actual applications.

Also, they are only available in Linux at the moment.

The DSi has two additional CPUs supported by BlocksDS:

- Teak: A DSP mostly used for audio (but rarely used in official games).
- Xtensa: A CPU present in the WiFi board of the DSi.

You can install both toolchains by running this:

```bash
wf-pacman -S toolchain-llvm-teak-llvm toolchain-gcc-xtensa-elf
```

When they are present, the build system of BlocksDS will build additional
libraries and examples.

### Clone the main repository of BlocksDS

Now, clone this repository:

```bash
git clone --recurse-submodules https://codeberg.org/blocksds/sdk.git
cd sdk
```

### Build the SDK

To build the SDK, run this from the root of this repository:

```bash
BLOCKSDS=$PWD make -j`nproc`
```

### Install the SDK

Now, you have some options. The first one is recommended, the others are for
users that may have advanced needs.

1. **Recommended**. Install it in your system in the default path. You won't
   need to set `BLOCKSDS` or `BLOCKSDSEXT` manually to use the SDK. Run:

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

{{% /steps %}}

### How to build projects

Now you can try to build one of the BlocksDS examples. They are available in
folder `examples` in the root of the repository. From there, for example, go to
`graphics_3d/volumetric_shadow` and type `make`. This will generate a NDS ROM
that you can try with an emulator or a flashcart.

You can also try one of the open source projects made with BlocksDS that are
listed [here](https://codeberg.org/blocksds/awesome-blocksds).
