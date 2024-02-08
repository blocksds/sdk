---
title: 'BlocksDS Documentation'
---

Welcome to the documentation of BlocksDS.

This is an [introduction](./introduction/introduction) to the SDK.

If you're looking for information on how to install BlocksDS, check the
[instructions](./setup/options).

If you're looking for documentation about specific libraries, check the
following links:

- **libnds**:

  - [API Information](./libnds/index.html)
  - [Examples](https://github.com/blocksds/sdk/tree/master/examples)

- **libteak**:

  - [API Information](./libteak/index.html)
  - [Examples](https://github.com/blocksds/sdk/tree/master/examples/dsp)

BlocksDS comes with sample Makefiles to build projects. They don't support many
common use-cases, like converting graphics and storing them in the filesystem.
They don't support things like having multiple DSP binaries either. You can
check [ArchitectDS](https://github.com/AntonioND/architectds) if you need a more
flexible build system.
