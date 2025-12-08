---
title: 'Additional DS libraries'
weight: 2
---

This is a list of DS-specific libraries that are supported in BlocksDS but not
installed by default. You can check their documentation and examples in their
respective homepages, but you can install them with `wf-pacman`.

### [NightFox’s Lib](https://github.com/knightfox75/nds_nflib)

Library designed to make development of 2D games easy, with support for sprites,
backgrounds (even "infinite" maps), and it also has limited 2D graphics support
using the 3D engine.

```sh
wf-pacman -Sy blocksds-nflib
```

### [Nitro Engine](https://github.com/AntonioND/nitro-engine)

Library to create 3D applications. It supports dual screen 3D, static and
animated models, supports all texture types, and has basic 2D support using the
3D hardware. It's compatible with NightFox's Lib.

```sh
wf-pacman -Sy blocksds-nitroengine
```

### [LibDSF](https://github.com/AntonioND/libdsf)

Library that uses pre-converted TTF fonts to render text. It can use the 3D
hardware of the NDS directly or render to buffers to be used by the 2D hardware.

```sh
wf-pacman -Sy blocksds-libdsf
```

### [Woopsi](https://github.com/AntonioND/Woopsi)

Woopsi is a GUI framework for the Nintendo DS, written in C++. The aim of the
project is to provide a windowing GUI that other programmers can use for their
applications, decreasing the tedious amount of custom code that they have to
write.

```sh
wf-pacman -Sy blocksds-libwoopsi
```

### [PAlib](https://github.com/AntonioND/palib)

PAlib (Programmer's Arsenal library) is a Nintendo DS library used to create
homebrew games. It is designed to be easily used by anyone. However, nowadays
there are better alternatives and PAlib should only be used to build old
projects that depend on it to be built.

```sh
wf-pacman -Sy blocksds-palib
```

### [µLibrary](https://github.com/AntonioND/ulibrary)

This is a library that provides functions to create 2D games using the 3D engine
of the Nintendo DS. The 3D GPU is more flexible than the 2D GPU.

```sh
wf-pacman -Sy blocksds-ulibrary
```

### [DSerial](https://github.com/asiekierka/dserial)

The final version of the DSerial card library, extracted from libdsmi and
adapted for BlocksDS.

```sh
wf-pacman -Sy blocksds-dserial
```

### [DSMI library](https://github.com/NitrousTracker/dsmi)

It provides MIDI support for the DS. The following communication methods are
supported: DSerial Edge, wireless access point (via DSMIDIWiFi).

```sh
wf-pacman -Sy blocksds-dsmi
```

### [gbajpeg](https://github.com/blocksds/gbajpeg)

Library originally designed to load JPEG files on GBA with very low resource
usage.

```sh
wf-pacman -Sy blocksds-gbajpeg
```
