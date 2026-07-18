---
title: 'Ported libraries'
weight: 3
---

This is a list of commonly-used C libraries that are supported in BlocksDS but
not installed by default. Some of them are provided by Wonderful Toolchain
itself, others are provided by BlocksDS. You can install them with `wf-pacman`.

### [libpng](https://libpng.org/pub/png/libpng.html)

libpng is the official PNG reference library. It supports almost all PNG
features, is extensible, and has been extensively tested for over 30 years

Install it with:

```sh
wf-pacman -Sy toolchain-gcc-arm-none-eabi-libpng16
```

Link it with:

```make
LIBS += -lpng -lz
```

### [zlib](https://www.zlib.net)

zlib is designed to be a free, general-purpose, legally unencumbered -- that is,
not covered by any patents -- lossless data-compression library for use on
virtually any computer hardware and operating system.

Install it with:

```sh
wf-pacman -Sy toolchain-gcc-arm-none-eabi-zlib
```

Link it with:

```make
LIBS += -lz
```

### [FreeType](https://freetype.org)

FreeType is a freely available software library to render fonts. It is written
in C, designed to be small, efficient, highly customizable, and portable while
capable of producing high-quality output (glyph images) of most vector and
bitmap font formats.

Install it with:

```sh
wf-pacman -Sy toolchain-gcc-arm-none-eabi-freetype
```

Link it with:

```make
LIBS += -lfreetype
```

### [Mbed TLS](https://github.com/Mbed-TLS/mbedtls)

Mbed TLS is a C library that implements X.509 certificate manipulation and the
TLS and DTLS protocols. Its small code footprint makes it suitable for embedded
systems.

Install it with:

```sh
wf-pacman -Sy blocksds-mbedtls
```

Link it with:

```make
LIBS    += -lmbedcrypto -lmbedtls -lmbedx509
LIBDIRS += $(BLOCKSDSEXT)/mbedtls
```

Check [this example](https://codeberg.org/blocksds/sdk/src/branch/master/examples/dswifi/get_website_ssl)
for more details.

### [libcurl](https://curl.se/libcurl)

libcurl is a free and easy-to-use client-side URL transfer library, supporting
many protocols and features. It is highly portable, thread-safe, IPv6
compatible, well documented and widely used by many companies.

Install it with:

```sh
wf-pacman -Sy blocksds-libcurl
```

Link it with:

```make
LIBS    += -lmbedcrypto -lmbedtls -lmbedx509 -lcurl -lz
LIBDIRS += $(BLOCKSDSEXT)/mbedtls $(BLOCKSDSEXT)/libcurl
```

Check [this example](https://codeberg.org/blocksds/sdk/src/branch/master/examples/dswifi/get_website_libcurl)
for more details.
