---
title: 'Tools'
weight: 4
---

This is a list of additional tools that don't come with the base SDK.  You can
check their documentation and examples in their respective homepages.

### [ptexconv](https://github.com/blocksds/ptexconv)

Tool to convert images to files in the format of the NDS. This tool is one of
the few available tools that can output files in the very efficient tex4x4
format used by the GPU of the NDS.

```sh
wf-pacman -Sy blocksds-ptexconv
```

### [ArchitectDS](https://github.com/AntonioND/architectds):

BlocksDS comes with sample Makefiles to build projects. They don't support many
common use-cases, like converting graphics and storing them in the filesystem.
They don't support things like having multiple DSP binaries either. You can
check ArchitectDS if you need a more flexible build system. It's written in
Python 3 and it uses [ninja-build](https://ninja-build.org/) internally.

### [NDS Compressors by CUE](https://github.com/WonderfulToolchain/wf-nnpack)

Compressors for Nintendo GBA/DS consoles by CUE. They support the formats that
the BIOS can decompress.

```sh
wf-pacman -Sy wf-nnpack
```

### [SuperFamiconv](https://github.com/Optiroc/SuperFamiconv)

A tile graphics converter with flexible and composable command line options. It
supports some ways to share graphics (palettes, tile sets) that grit doesn't
support.

```sh
wf-pacman -Sy wf-superfamiconv
```

### [SoX](https://sourceforge.net/projects/sox/)

SoX is the Swiss Army Knife of sound processing utilities. It can convert audio
files to other popular audio file types and also apply sound effects and filters
during the conversion.

```sh
wf-pacman -Sy wf-sox
```
