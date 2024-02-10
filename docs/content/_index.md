---
title: 'BlocksDS Documentation'
---

Welcome to the documentation of BlocksDS.

This is an [introduction](./introduction/introduction) to the SDK.

If you're looking for information on how to install BlocksDS, check the
[instructions](./setup/options).

If you're looking for documentation about specific libraries, check the
following links:

- **libnds**: General-purpose library to use the hardware of the NDS.

  - [API Information](./libnds/index.html)
  - [Examples](https://github.com/blocksds/sdk/tree/master/examples)

- **libteak**: Library to use the Teak DSP included in the DSi and 3DS consoles.

  - [API Information](./libteak/index.html)
  - [Examples](https://github.com/blocksds/sdk/tree/master/examples/dsp)

- **libxm7**: Hardware-accelerated player of XM and MOD music files that runs
  on the ARM7.

  - [API Information](./libxm7/index.html)
  - [Example](https://github.com/blocksds/sdk/tree/master/examples/audio/libxm7)

- **dswifi**: Library to use the WiFi hardware of the NDS. It has no support for
  the WiFi improvements of the DSi.

  - [API Information](./dswifi/index.html)

BlocksDS comes with sample Makefiles to build projects. They don't support many
common use-cases, like converting graphics and storing them in the filesystem.
They don't support things like having multiple DSP binaries either. You can
check [ArchitectDS](https://github.com/AntonioND/architectds) if you need a more
flexible build system.
