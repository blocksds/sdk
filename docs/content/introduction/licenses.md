---
title: 'Licenses'
weight: 0
---

This document enumerates the components provided by BlocksDS and their licenses.
This is useful if you want to release a binary built with BlocksDS and you
aren't sure about what copyright notices you need to add.

## DSWiFi

This is used in the default ARM7 binary.

MIT license. Copyright notice required:

```
Copyright (C) 2005-2006 Stephen Stair - sgstair@akkit.org
```

## libnds

This is used in all programs that use the default crts, ARM7 binary, etc.

Zlib license. No copyright notice required in binary distributions.

FatFs uses a custom BSD/MIT/ISC-like license that doesn't require attribution in
binary files either.

## LibXM7

This is only used if the user explicitly links it.

MIT license. Copyright notice required:

```
Copyright (C) 2018 sverx
```

## Maxmod

This is used in the default ARM7 binary.

ISC license. Copyright notice required:

```
Copyright (c) 2008, Mukunda Johnson (mukunda@maxmod.org)
```

## picolibc

This is always used unless you tell the compiler to not link libc at all.

Take a look at `COPYING.picolibc` in the `picolibc` repository
[here](https://github.com/WonderfulToolchain/wf-picolibc/blob/wonderful/COPYING.picolibc).

## libstdc++

This is used in any C++ program or program that is linked with a C++ library.

It uses license GPL 3.0 with an exception for any program compiled with GCC.
That means that, as long as you use GCC as your compiler, you can license your
compiled application under any license you want, or make it closed source. If
you use any other compiler, you need to license your application under the
GPL 3.0.

https://www.gnu.org/licenses/gcc-exception-3.1.en.html

## crts

This is always used unless the user selects different ones.

Mozilla Public License v2.0. No copyright notice required.

## default\_arm7

This is used in any project that doesn't include source code for an ARM7 binary.

Zlib license. No copyright notice required in binary distributions.
