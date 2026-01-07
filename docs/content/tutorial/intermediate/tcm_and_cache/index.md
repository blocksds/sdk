---
title: TCM and Cache
weight: 2
---

## 1. Introduction

The data and instruction caches and the DTCM and ITCM memory regions aren't
things you need to worry about a lot, but they interact in unintuitive ways with
other components of the DS, so it's important to introduce them now so that you
can be careful about them when more advanced topics are introduced, like loading
files from the filesystem, using the 3D hardware, communications between CPUs...

This chapter won't show you any example of any of the issues, but it will
introduce the concepts so that they can be applied later. It's ok if this is too
much right now, you can come back to this chapter later when you have seen some
problematic situations.

## 2. Caches

Caches are intermediate memories between the CPU and RAM that are usually
managed automatically by the CPU and speed up accesses to RAM.

The data cache is an intermediate memory between the ARM9 and main RAM that is a
lot faster than main RAM, but quite small (only 4 KB). It is used to hold parts
of main RAM that have been accessed recently so that it isn't required to access
main RAM all the time. If you have just read a variable from main RAM, a small
chunk of RAM containing that variable will be loaded to the data cache and it
will be kept there until the code moves on, it hasn't used that variable for a
while, and the cache controller decides to replace it with something else.

The instruction cache is similar (and bigger: 8 KB), but for code. This can be
very useful when executing loops, for example. The first iteration is slow
because the CPU needs to load the code of the loop to the cache, but all other
iterations will be faster because the code is already in the cache.

There are some important things to consider:

- DMA (which we'll use in the next chapter of the tutorial) is hardware created
  specifically to copy data around. However, it can't see any data present in
  the cache. You'll learn how to manage the cache in this kind of situations
  with some examples in the DMA chapter.

- If you're sharing a buffer between the ARM9 and the ARM7, the ARM7 won't be
  able to see data stored in the ARM9 cache. This means that, for example, the
  ARM7 may never see a "ready" flag set by the ARM9 unless you know how to
  update main RAM.

Bugs caused by bad cache management are hard to debug because they tend to be
unpredictable. However, disabling the caches has a huge performance cost, so
it isn't a very realistic option.

## 3. Tightly Coupled Memory (TCM)

There are two regions of memory called DTCM (Data TCM, 16 KB) and ITCM
(Instruction TCM, 32 KB) that are connected to the ARM9 and have very low
latency. It is a very good idea to use them for code that you need to make very
fast, or data that is accessed a lot. In fact, the stack is placed in DTCM by
default. Note that the ARM7 can't access DTCM or ITCM!

The main difference with the cache is that you need to manage DTCM and ITCM by
hand. There are some macros in libnds that you can use to place code or data in
DCTM and ITCM.

```c
/// Used to place a function in ITCM
#define ITCM_CODE __attribute__((section(".itcm.text"), long_call))
/// Used to place initialized data in ITCM
#define ITCM_DATA __attribute__((section(".itcm.data")))
/// Used to place uninitialized data in ITCM
#define ITCM_BSS  __attribute__((section(".itcm.bss")))

/// Used to place initialized data in DTCM
#define DTCM_DATA __attribute__((section(".dtcm")))
/// Used to place uninitialized data in DTCM
#define DTCM_BSS __attribute__((section(".sbss")))
```

DTCM can only hold data (the CPU can't fetch instructions from it), but ITCM can
hold code and data because of how the ARM architecture works. For example,
functions usually have a pool of constants at the end, and the code itself loads
constants from the pool. You don't need to worry about this, though. The
compiler takes care of this kind of details for you.

If you want to place things in ITCM or DTCM, you can do it this way:

```c
ITCM_CODE __attribute__((noinline))
int test_function(int a, int b)
{
    return a + b;
}

DTCM_DATA int dtcm_data_var = 12345;

DTCM_BSS int dtcm_bss_var = 0;
```

BSS is used for variables that start as zero, DATA is used for variables that
don't start as 0. The `noinline` attribute may be needed in some functions if
the compiler chooses to inline them inside functions placed in main RAM. This
can be a problem with small functions, but it can also happen with big functions
in some cases (for example, a `static` function that is only called from one
other function in the same file is likely to be inlined).

One last thing to consider is that placing data in DTCM will limit how much
stack there is available for your application.

Normally, DTCM BSS and DATA are placed right at the start of DTCM. The stack
starts from the end of DTCM and it grows downwards. This configuration allows it
to overflow into main RAM if it grows too much. However, if you allocate
variables in DTCM, the stack will be restricted. The libraries included in
BlocksDS don't place any variables in DTCM.

There isn't an easy way to place your variables at the end of DTCM
automatically, but BlocksDS gives you the option of setting the value of a
symbol called `__dtcm_data_size` to hardcode the total size of DTCM used for
user variables. If you do it, the linker will be able to place your variables at
the end of DTCM and make it possible for the stack to grow into main RAM again.

You can set the value of this symbol by adding the following to `LDFLAGS` in
your Makefile. For example, this will leave 512 bytes of DTCM for the code:
`-Wl,--defsym=__dtcm_data_size=512`

## 4. DMA copy while the CPU runs

DMA copies stall the CPU when it tries to access the same memory as the DMA
hardware. This means that, in theory, it's possible to do DMA copies while the
CPU is doing other work as long as the CPU doesn't try to use the same memory as
the DMA.

In the case of the ARM9, you can achieve this by placing your code in ITCM and
your variables in DTCM. DMA can't see ITCM or DTCM because they are internal to
the ARM9, so DMA copies will involve main RAM, VRAM, or I/O registers. If the
ARM9 only uses internal memory it won´t get stalled by DMA.

In practice, it isn't easy to find applications for this. Generally you use DMA
when you want to copy big chunks of data or when you're doing I/O DMA writes
(to achieve scanline-based effects, for example). If you're copying big chunks
of data there won't be anything useful you can do at the same time. If you're
doing small I/O writes, setting this up isn't worth the effort.
