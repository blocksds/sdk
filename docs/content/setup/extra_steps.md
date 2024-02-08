---
title: 'Extra steps'
weight: 40
---

## 1. Test

To test that everything is working, you can try building one of the templates.

There is a template that lets the programmer write the code for both CPUs of the
NDS (`templates/rom_combined`) and a template that uses the default ARM7 core
that comes with Blocks DS and lets the programmer focus on the ARM9 core. There
are also two templates to create static libraries.

For example, go to the combined template folder and run:

```bash
cd templates/rom_combined
make
```

This should have generated a NDS ROM that you can run on any emulator or
flashcart.

To do a verbose build, run `make` like this (this also works for the Makefile of
the SDK):

```bash
VERBOSE=1 make
```

**NOTE:** The paths in the Makefile that refer to source code, includes,
graphics, data, etc, must be inside the folder of the project. That means you
can't use `..` in a path to go one level up from the Makefile. If you really
need to use folders outside of the folder of the project, create a symlink to
the destination, or build the other code as a static library and link it with
the project.

## 2. Additional tools and libraries

This section has a list of tools and libraries that support BlocksDS and can be
installed in `/opt/blocksds/external`:

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

Tools:

- [ptexconv](https://github.com/blocksds/ptexconv): Tool to convert images to
  files in the format of the NDS. This tool is one of the few available tools
  that can output files in the very efficient tex4x4 format used by the GPU of
  the NDS.

## 3. Keeping multiple versions of BlocksDS

You may keep multiple versions of this SDK in your PC. The location of the
active SDK is stored in the environment variables `BLOCKSDS` and `BLOCKSDSEXT`,
so all you need to do is to change their values and point to the version of the
SDK you want to use.

By default, the paths are assumed to be `/opt/blocksds/core/` and
`/opt/blocksds/external/`. The first one is used for core BlocksDS libraries
and tools, and it is managed by BlocksDS. The second one is left for users to
freely install third party libraries and tools.
