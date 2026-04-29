---
title: 'Base libraries'
weight: 1
---

All the base libraries included with BlocksDS come with extensive documentation
and examples. If you want information about other libraries not included with
the base SDK, check the links [here](..).

### libnds

General-purpose library to use the hardware of the NDS.

{{< cards cols="2" >}}
  {{< card
    icon="book-open"
    title="Documentation"
    link="../../../libnds/index.html"
  >}}
  {{< card
    icon="code"
    title="Examples"
    link="https://codeberg.org/blocksds/sdk/src/branch/master/examples"
  >}}
{{< /cards >}}

This library is linked by default. You may need to do do it manually if you
modify the Makefile. Use `-lnds9` in the ARM9, `-lnds7` in the ARM7:

```make
LIBS    += -lnds9
LIBDIRS += $(BLOCKSDS)/libs/libnds
```

### Maxmod

Music player that can play MOD, S3M, XM, IT and WAV files. It can use hardware
or software mixing, and it runs on the ARM7.

{{< cards cols="2" >}}
  {{< card
    icon="book-open"
    title="Documentation"
    link="../../../maxmod/index.html"
  >}}
  {{< card
    icon="code"
    title="Examples"
    link="https://codeberg.org/blocksds/sdk/src/branch/master/examples/maxmod"
  >}}
{{< /cards >}}

This library is linked by default. You may need to do do it manually if you
modify the Makefile. Use `-lmm9` in the ARM9, `-lmm7` in the ARM7:

```make
LIBS    += -lmm9
LIBDIRS += $(BLOCKSDS)/libs/maxmod
```

### DSWiFi

Library to use the WiFi hardware of the NDS and DSi with support for local
multiplayer and Internet access.

{{< cards cols="2" >}}
  {{< card
    icon="book-open"
    title="Documentation"
    link="../../../dswifi/index.html"
  >}}
  {{< card
    icon="code"
    title="Examples"
    link="https://codeberg.org/blocksds/sdk/src/branch/master/examples/dswifi"
  >}}
{{< /cards >}}

Link it with `-ldswifi9` in the ARM9 and `-ldswifi7` in the ARM7:

```make
LIBS    += -ldswifi9
LIBDIRS += $(BLOCKSDS)/libs/dswifi
```

### LibXM7

Hardware-accelerated player of XM and MOD music files that runs on the ARM7.

{{< cards cols="2" >}}
  {{< card
    icon="book-open"
    title="Documentation"
    link="../../../libxm7/index.html"
  >}}
  {{< card
    icon="code"
    title="Examples"
    link="https://codeberg.org/blocksds/sdk/src/branch/master/examples/libxm7"
  >}}
{{< /cards >}}

Link it with `-lxm79` in the ARM9 and `-lxm77` in the ARM7:

```make
LIBS    += -ldswifi9
LIBDIRS += $(BLOCKSDS)/libs/libxm7
```

### libteak

Library to use the Teak DSP included in the DSi and 3DS consoles.

{{< cards cols="2" >}}
  {{< card
    icon="book-open"
    title="Documentation"
    link="../../../libteak/index.html"
  >}}
  {{< card
    icon="code"
    title="Examples"
    link="https://codeberg.org/blocksds/sdk/src/branch/master/examples/dsp"
  >}}
{{< /cards >}}

Link it with:

```make
LIBS    += -lteak
LIBDIRS += $(BLOCKSDS)/libs/libteak
```

### Grit

Graphics converter for GBA and NDS.

{{< cards cols="2" >}}
  {{< card
    icon="book-open"
    title="Documentation"
    link="../../../grit/index.html"
  >}}
  {{< card
    icon="code"
    title="Repository"
    link="https://codeberg.org/blocksds/grit"
  >}}
{{< /cards >}}
