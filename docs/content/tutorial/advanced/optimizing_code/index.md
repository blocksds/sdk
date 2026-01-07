---
title: Optimizing code
weight: 3
---

## 1. Introduction

This chapter mentions a few techniques you can use to optimize code for the DS
specifically, not general techniques that apply to all code in general.

## 2. Profiling code

Before you try to optimize any code, you should make sure you can compare how
fast the old and new versions are. There are a few options you can use depending
on the level of accuracy you want:

- Create an FPS counter and look for framerate drops. This is only useful for
  applications that are really slow and sometimes go below 60 FPS.

- Check the value of `REG_VCOUNT` when you start running the code you want to
  profile, and after the code has finished. The difference is the number of
  scanlines that the code has needed to run. This isn't very accurate, but it
  can be good enough in many cases. For example, you could use it to see how
  much time per frame you require to simulate the physics of your game and how
  much time is spent rendering the graphics. This kind of things usually take a
  significant amount of time per frame, so you may see that your code takes tens
  of scanlines to finish.

- Use hardware timers to calculate how much time some code needs to run to the
  maximum accuraccy supported by the DS. This can be used to measure everything,
  from single functions to how long your game loop runs for. Libnds has a few
  functions to help you do this:

  - `cpuStartTiming(0)` starts two timers running in cascade mode. If you select
    timer 0, it will use timers 0 and 1.

  - `cpuEndTiming()` stops the timers and returns the number of timer ticks that
    have passed since `cpuStartTiming()`. You can use `timerTicks2usec()` to
    convert it to microseconds if you want.

  You can see an example of using this system here:
  [`examples/time/profiling`](https://github.com/blocksds/sdk/tree/master/examples/time/profiling)

## 3. ARM and Thumb CPU modes

By default BlocksDS asks GCC to compile all code as Thumb code, not ARM. The
two CPU modes have a few differences:

ARM:

- Instructions are 32-bit wide.
- All instructions can be executed conditionally.
- It supports multiplications from 32-bit wide operands into a 64-bit wide
  result. This is also used by compilers to optimize divisions by constants.
- All CPU registers are treated equally.
- The CPU mode and status can be changed in this mode.

Thumb:

- Instructions are 16-bit wide.
- The only conditional instructions are jumps.
- Multiplications have a 32-bit wide result.
- Registers `r0` to `r7` are more convenient to use than the rest.
- The CPU mode and status can't be changed.

In general, this means that ARM code can do more things per instruction, but at
a cost of needing more space for the code. Reading code from memory takes time,
so the extra size of ARM functions needs to be considered when switching
functions from Thumb to ARM.

The ARM9 has an instruction cache, so keeping your code small can speed it up
significantly: the cache will have to swap code in and out less frequently.
Also, the ARM9 has ITCM, where you can place code that needs to be particularly
fast. If you want more details, check the chapter about the
[TCM and cache](../../intermediate/tcm_and_cache).

There are two options to tell GCC to build your code as ARM:

- You can tag the implementation of your functions with `ARM_CODE`. The default
  is Thumb, which is equivalent to using `THUMB_CODE`:

  ```c
  ARM_CODE int test_function(int a, int b)
  {
      return a + b;
  }

  THUMB_CODE int test_function2(int a, int b)
  {
      return a + b;
  }
  ```

- If you're using the default makefiles of BlocksDS You can rename your `.c` and
  `.cpp` files to `.arm.c` and `.arm.cpp`. All the code in that file will be
  built as ARM (unless `THUMB_CODE` is used).

When should you use ARM code?

- You have a function that does a lot of multiplications (for example, for 3D
  graphics) that runs very frequently.
- You have a big and complicated function with many variables that needs to be
  very fast.

However, making GCC build your functions as ARM won't always make your code
faster. Very simple functions won't benefit from it, or functions that are
executed very rarely. Sometimes, moving your Thumb functions to ITCM is enough
to make it fast, and building it as ARM won't help that much.

Remember to always profile your code before and after making this kind of
changes. Optimizing rarely used functions isn't useful. Optimizing your most
frequently used functions will have a very big effect.

## 4. ITCM and DTCM

ITCM and DTCM are very fast memory areas where you can store ARM9 code and
variables. We learned most things about ITCM and DTCM in chapter
[TCM and cache](../../intermediate/tcm_and_cache). Check that chapter for more
information.

{{< callout type="error" >}}
This chapter is a work in progress...
{{< /callout >}}
