---
title: 'Licenses'
weight: 4
---

This document enumerates the components which are part of a BlocksDS toolchain
and their licenses. This is useful if you want to release a binary built with
BlocksDS and you aren't sure about what copyright notices you need to add.

Note that this document **does not constitute legal advice**. It is a simplified
enumeration of project licenses, not a grant of licensing terms in its own right.
In no event will the BlocksDS project or its authors be held liable for any
damages arising from errors or mistakes in this information.

## Distributed by BlocksDS

These libraries are present in the BlocksDS source tree.

### DSWiFi

This is used in the default ARM7 binary.

The library itself is under the MIT license. You must include the following
copyright notice with your program, as well as the MIT license terms:

```
Copyright (C) 2005-2006 Stephen Stair - sgstair@akkit.org
Copyright (c) 2006-2018 Dave Murphy (WinterMute)
Copyright (c) 2021 Max Thomas (ShinyQuagsire)
Copyright (C) 2025 Antonio Niño Díaz
```

It also includes code of libraries [lwIP](https://savannah.nongnu.org/projects/lwip/)
and [Mbed TLS](https://github.com/Mbed-TLS/mbedtls). Mbed TLS is always
included, lwIP can be included or not depending on your needs.

Mbed TLS files are provided under a dual [Apache-2.0](https://spdx.org/licenses/Apache-2.0.html)
OR [GPL-2.0-or-later](https://spdx.org/licenses/GPL-2.0-or-later.html) license.
This means that users may choose which of these licenses they take the code
under. Normally you will use the Apache-2.0 license, so you need to provide the
Apache-2.0 license text, and you need to include the following copyright notice
in your program:

```
Copyright The Mbed TLS Contributors
```

On the ARM9 you can use a version of the library with and without lwIP (required
for Internet communications). If you use the version of the library with lwIP
you also need to follow the terms of the lwIP library, licensed under the
3-clause BSD license:

```
Copyright (c) 2001, 2004 Swedish Institute of Computer Science.
```

### libnds

This is used in all programs that use the default crts, ARM7 binary, etc.

Zlib license. No copyright notice required in binary distributions.

FatFs uses a custom BSD/MIT/ISC-like license that doesn't require attribution in
binary files either.

### LibXM7

This is only used if the user explicitly links it.

MIT license. You must include the following copyright notice with your program,
as well as the MIT license terms:

```
Copyright (C) 2018 sverx
```

### Maxmod

This is used in the default ARM7 binary.

ISC license. You must include the following copyright notice with your program:

```
Copyright (C) 2008-2009 Mukunda Johnson (mukunda@maxmod.org)
Copyright (C) 2021-2025 Antonio Niño Díaz
Copyright (C) 2023 Lorenzooone (lollo.lollo.rbiz@gmail.com)
```

### crts

This code is used in any program unless the user explicitly provides an alternative.

[Mozilla Public License v2.0](https://www.mozilla.org/en-US/MPL/2.0/).

> You must inform recipients that the Source Code Form of the Covered Software is governed by the terms of this License, and how they can obtain a copy of this License.

The source code for crts is available [here](https://github.com/blocksds/sdk/tree/master/sys/crts).

### Default ARM7

This is used in any project that doesn't include source code for an ARM7 binary.

The core itself is licensed under the Zlib license. No copyright notice required
in binary distributions.

However, the default ARM7 core uses DSWiFi and Maxmod, so you need to give
credit according to their licenses. Other ARM7 cores use other combinations of
libraries. For example, some use LibXM7 instead of Maxmod, and some don't use
DSWiFi at all. You need to only give credit for the libraries present in your
core.

## Distributed by the toolchain

These libraries are not present in the BlocksDS source tree or packages,
but are used when creating a BlocksDS application.

### picolibc

This library is always used unless you tell the compiler to not link libc at all.

picolibc is a fork of newlib licensed under a variety of MIT- and BSD-like licenses.
Citing [the README](https://github.com/picolibc/picolibc/blob/main/README.md):

> While much of the code comes from Newlib, none of the GPL-related bits used to build
> the library are left in the repository, so all of the source code uses BSD-like licenses,
> a mixture of 2- and 3- clause BSD itself and a variety of other (mostly older) licenses
> with similar terms.

For more information, take a look at `COPYING.picolibc` in the `picolibc` repository
[here](https://github.com/WonderfulToolchain/wf-picolibc/blob/wonderful/COPYING.picolibc).

### libstdc++

This library is used, by default, in any C++ program or program that is linked with
a C++ library, unless you tell the compiler to not link default libraries
at all.

It uses the GPL 3.0 license with a runtime library exception. This exception
allows libstdc++ to be used in non-GPL-licensed binaries without infringing
on the license; citing the FAQ:

> The special exception permits use of the library in proprietary applications.

> [...] what restrictions are there on programs that use the library? None.

One notable exception is using proprietary forks of GCC or proprietary tools
which process GCC's intermediary code; however, neither of these actions are
done by a standard BlocksDS configuration.

For more information, read [the license terms of the exception](https://www.gnu.org/licenses/gcc-exception-3.1.en.html).

### libgcc

This library is used, by default, in any program compiled with GCC.

It uses the GPL 3.0 with the same [runtime library exception](https://github.com/gcc-mirror/gcc/blob/master/COPYING.RUNTIME)
as libstdc++.
