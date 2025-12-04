---
title: 'Additional libraries/tools'
weight: 10
---

There are also some libraries and tools that are supported in BlocksDS, but not
fully integrated with it. They are listed below.

Libraries:

- [NightFox’s Lib](https://github.com/knightfox75/nds_nflib): Library designed
  to make development of 2D games easy, with support for sprites, backgrounds
  (even "infinite" maps), and it also has limited 2D graphics support using the
  3D engine.

- [Nitro Engine](https://github.com/AntonioND/nitro-engine): Library to create
  3D applications. It supports dual screen 3D, static and animated models,
  supports all texture types, and has basic 2D support using the 3D hardware.
  It's compatible with NightFox's Lib.

- [gbajpeg](https://github.com/blocksds/gbajpeg): Library designed to load
  JPEG files on GBA with very low resource usage, which also makes it a good
  library for NDS.

- [LibDSF](https://github.com/AntonioND/libdsf): Library to render text using
  the 3D hardware of the NDS.

Tools:

- [ptexconv](https://github.com/blocksds/ptexconv): Tool to convert images to
  files in the format of the NDS. This tool is one of the few available tools
  that can output files in the very efficient tex4x4 format used by the GPU of
  the NDS.

- [ArchitectDS](https://github.com/AntonioND/architectds): BlocksDS comes with
  sample Makefiles to build projects. They don't support many common use-cases,
  like converting graphics and storing them in the filesystem. They don't
  support things like having multiple DSP binaries either. You can check
  ArchitectDS if you need a more flexible build system.

