---
title: 'Optimization guide'
weight: 60
---

This guide covers some techniques one can use to optimize DS/DSi homebrew
with BlocksDS.

## Optimizing CPU usage

* **Measure CPU performance on hardware only!** As of writing (May 2024), the most
  popular emulators are not sufficiently cycle-accurate to provide accurate
  profiling results; this might lead, at worst, to mistaken optimizations.
* Avoid using floating point values (`float`, `double`). As the DS does not
  include any hardware for accelerating floating point math, operations on them
  have to be emulated in software, which is quite costly.
* On the ARM9, place "hot" (often used) code and data in ITCM and DTCM,
  respectively. More information is available below.

### ITCM and DTCM

ITCM stands for *Instruction Tightly Coupled Memory*, and on the DS refers to
32 kilobytes of fast memory connected directly to the ARM9 CPU to be used for
instruction code. Analogously, DTCM refers to 16 kilobytes of
*Data Tightly Coupled Memory*.

To place a function in ITCM or DTCM, you can use one of two approaches:

* Rename the .c or .cpp file containing it to use the .itcm.c, .itcm.cpp,
  .dtcm.c or .dtcm.cpp suffix instead - to move the file's entire contents,
* Annotate the specific function with one of the following macros:
  * `ITCM_CODE` - for code,
  * `DTCM_DATA` - for data pre-initialized with values,
  * `DTCM_BSS` - for data which is not pre-initialized or is filled with
    zeros.

Note that neither ITCM nor DTCM are accessible to hardware external to the
ARM9 main CPU, such as the ARM7 or the DMA hardware.

More details, as well as information on how to use ITCM/DTCM in assembly
code, is available in the [usage notes](../usage_notes).

## Reducing memory usage

* By default, the versions of `printf()` and `scanf()` linked by the toolchain
  contain code to handle `float` and `double` values. The required conversion
  code costs a few kilobytes of memory. Instead, integer versions can be
  utilized, as documented below.

### Integer versions of stdio.h functions

By default, the build of `picolibc` of BlocksDS makes `printf()`, `sscanf()` and
similar functions support float and double conversions. This is done to maintain
compatibility with existing code (and the C standard), but it increases the size
of the final binaries. It is possible to switch to integer-only versions of the
functions by adding the following line to the `DEFINES` of your Makefile:

```make
DEFINES := [all other options go here] \
    -DPICOLIBC_LONG_LONG_PRINTF_SCANF
```

There are additional variants available, depending on the codebase's specific
requirements:

* `-DPICOLIBC_DOUBLE_PRINTF_SCANF`: the default; supports float and double values.
* `-DPICOLIBC_FLOAT_PRINTF_SCANF`: supports float values, but not doubles.
* `-DPICOLIBC_LONG_LONG_PRINTF_SCANF`: supports all integer types, but not float types.
* `-DPICOLIBC_INTEGER_PRINTF_SCANF`: supports 32-bit integer types, but not 64-bit integer types such as `long long` or float types.
* `-DPICOLIBC_MINIMAL_PRINTF_SCANF`: the smallest version; supports only 32-bit integer types and omits support for many non-basic
  printf/scanf features. Full compatibility with `libnds` is not guaranteed at this level, and it should be used with caution.

For more information, please read [the relevant picolibc documentation](https://github.com/picolibc/picolibc/blob/main/doc/printf.md).

## Optimizing filesystem usage

If you've programmed for ROM-based platforms before, like the GBA, you might be
used to accessing in-ROM assets or filesystems instantly. That is not the case
on BlocksDS; only data loaded to memory is fast to access, while filesystem
reads including NitroFS are comparatively slow. Note that the speed also differs
depending on hardware. Emulators and the DSi SD card are faster than commonly
available flashcarts. Some SD cards are also slower than others. In addition,
fragmented FAT filesystems can be slower to read than defragmented ones.
As such, one should not rely on filesystem accesses being of a particular
minimum speed.

There are things which can be done to make the most out of filesystem I/O:

* Read larger chunks of data at a time. As starting an SD card read has latency
  of its own, processing more than one sector in one read call is much faster
  than doing so across multiple read calls. An optimal value is the size of
  one cluster (typically between 4 and 32 KB), though BlocksDS can also
  sometimes optimize reads larger than that.
* If your data format is based around small sequential reads, you may benefit
  from enabling the C standard library's buffering mechanism using the `setvbuf`
  function. Note that this slows down random reads, as a buffered random read will
  require re-filling the whole buffer. For example, to allocate a 4 KB buffer for
  a file, one can use:

```c
    setvbuf(file, NULL, _IOFBF, 4 * 1024);
```

* If your data format is based around random reads, you may benefit from enabling
  a lookup cache. This reduces the amount of SD card reads required for finding the
  location of the specific chunk of data in the filesystem. The lookup cache has to
  be of a certain maximum size, but on defragmented devices will typically not
  exceed a few hundred bytes. Note that the lookup cache is automatically enabled
  for NitroFS accesses. For example, to try and enable the lookup cache on a file:

```c
    fatInitLookupCacheFile(file, 2 * 1024); /* 2 KB maximum lookup cache */
```

## OpenGL-like API usage

### Texture formats

The `GL_RGB` texture format isn't actually supported by the hardware. When a
texture is loaded with `glTexImage2D()` with format `GL_RGB`, it is actually
converted to `GL_RGBA` internally by setting the alpha bit of all pixels to 1.
This is pretty slow, and it isn't normally needed because any graphics
conversion tool (such as `grit`) can set the alpha bit to 1 when the image is
converted. None of the 3D examples of BlocksDS use `GL_RGB`. The ones that use
`GL_RGBA` also show how to convert images the right way.

Also, consider using other texture formats. `GL_RGBA` isn't a very efficient way
to store textures. All other formats (the ones that use palettes) use a lot less
memory. In many cases `GL_COMPRESSED` is the best format you can use (but
converting images to this format is only currently supported by `ptexconv`).

BlocksDS has examples of how to use all texture formats (except for `GL_RGB`,
which works the same way as `GL_RGBA`, and is never used in order to not
encourage new users to use it). Take a look at the examples for more details.

Final note: The video capture circuit of the NDS sets the alpha bit to 1, so it
isn't needed even if you want to use the saved image as a texture, or as a
16-bit bitmap background. There is an example of how to render to a texture too.
