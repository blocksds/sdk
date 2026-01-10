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
  [`examples/time/profiling`](https://codeberg.org/blocksds/sdk/src/branch/master/examples/time/profiling)

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

You can also use a website called [Godbolt](https://godbolt.org/) to see the
resulting assembly code and get an idea of how good it is. Remember to use the
following settings:

- As compiler, select `ARM GCC 15.2.0` (or the version that you're currently
  using).

- As build options, use:

  - ARM9 code: `-O3 -mfloat-abi=soft -mthumb -mcpu=arm946e-s+nofp`
  - ARM7 code: `-O3 -mfloat-abi=soft -mthumb -mcpu=arm7tdmi`

This website lets you play around with the code and see the results right away,
which is very useful. However, sometimes it can be hard to copy your code into
that website because it depends on external libraries, etc.

You can add `-save-temps` or `-S` to `CFLAGS` and `CXXFLAGS` in the makefile of
your projects and build it. It will create `.s` files with the assembly code
generated for every `.c` and `.cpp` files as they are built.

## 4. Divisions by constants

The CPUs of the DS support hardware multiplications, but not divisions. The ARM9
has a hardware divider that you can use for divisions in general, but there is a
way to turn divisions by constants into multiplications.

This system requires multiplications of 32 bit operands into a 64 bit result, so
it's mostly useful when running in ARM mode, which has the `umull` and `smull`
instructions. Thumb only has `mul`, and it will emit calls to functions like
`__aeabi_uidiv()` instead, which is a lot slower.

```c
#define ARM_CODE __attribute__((target("arm")))

// Optimized to a multiplication.
ARM_CODE unsigned int divide_by_3_arm(unsigned int value)
{
    return value / 3;
}

// Not optimized, this will call a routine to do the division.
unsigned int divide_by_3_thumb(unsigned int value)
{
    return value / 3;
}
```

With Godbolt you can see the following result (with the ARM9 settings of the
previous section):

```asm
divide_by_3_arm:
        ldr     r3, .L3
        umull   r2, r0, r3, r0
        lsr     r0, r0, #1
        bx      lr
.L3:
        .word   -1431655765

divide_by_3_thumb:
        push    {r4, lr}
        movs    r1, #3
        bl      __aeabi_uidiv
        pop     {r4, pc}
```

How does this system work?

1. We start with the following division:

   ```
   x = a / 3
   ```

   In assembly, if there was a division instruction, this would correspond to
   something like:

   ```
   mov   b, #3
   div   x, a, b
   ```

2. We convert it into a multiplication by the reciprocal of the constant 3.

   ```
   x = a * 1 / 3
   ```

   However, when using integer values, `1 / 3` is 0, so this change alone won't
   work. It would work if we were using floating point, but there is no floating
   point hardware accelerator in the DS.

3. The solution is to use fixed point for the calculations. We can shift the
   reciprocal by 32 bits and shift the result back by 32 bits.

   ```
   x = [a * ((1 << 32) / 3)] >> 32
   ```

   Why 32 bits exactly? Because a shift by 32 bits is free in assembly, so the
   resulting code would look like this:

   ```
   mov   b, #((1 << 32) / 3)
   umull y, x, a, b
   ```

   In this case, the result of multiplying registers `a` and `b` is stored in
   `x` (the top 32 bits) and `y` (the bottom 32 bits). All you need to do is to
   ignore the value saved to `y` and treat it as temporary storage. The compiler
   can do this automatically.

## 5. Bit twiddling hacks

There are many tricks to optimize common bitwise operations. For example, if you
want to count the number of bits set in an integer, you may be tempted to write
a loop that checks each bit individually. However, there are people that have
created optimized code for this kind of common operations.

If you're interested in this, check the following website:
https://graphics.stanford.edu/~seander/bithacks.html

## 6. ITCM and DTCM

ITCM and DTCM are very fast memory areas where you can store ARM9 code and
variables. We learned most things about ITCM and DTCM in chapter
[TCM and cache](../../intermediate/tcm_and_cache). Check that chapter for more
information.

## 7. Copying data quickly. DMA or CPU?

If you're copying a lot of data you may be interested in optimizing it. One of
the first things that you may think about is to use DMA to copy the data instead
of using `memcpy()` or similar functions.

There are a few situations in which using DMA to copy data is a good idea:

- You're copying data to VRAM from main RAM, and the data in main RAM has been
  embedded with `#include` as binary data in your program.

- You're trying to setup a copy with special triggers, like copying just a bit
  of data every horizontal/vertical blanking period.

However, DMA copies aren't much faster than an optimized CPU `memcpy()` (like
the one included in libnds), and they have big disadvantages:

- DMA can't access the ARM9 cache, so you need to flush the source and
  flush the destination (or invalidate the destination if it's aligned to cache
  lines). The flush/invalidate operations take some time to finish, so it may
  not be faster than `memcpy()`.

- DMA copies from/to main RAM prevent the CPU from accessing main RAM during the
  copy. This means that in most cases the CPU can't do anything while a DMA copy
  is going on, so you might as well use the CPU to do the copy.

Cearn wrote a good comparison of different copy methods in
[this](https://gbadev.net/tonc/text.html#ssec-demo-se2) chapter of Tonc. Note
that the `memcpy()` of libnds behaves more like `memcpy32()` than the `memcpy()`
of the table.

In general, you should stick to `memcpy()` unless you have determined that it is
slowing your code down, and you should always profile the code with `memcpy()`
and with DMA (plus cache flushes/invalidates) to see if you're actually making
the code faster.

## 8. Organizing data to optimize cache accesses

Accessing main RAM is slow, and it can be quite noticeable in code running on
the ARM9 (the ARM7 is slower, so it isn't less noticeable there). The ARM9 has
data and instruction caches to help with this. Check the chapter about
[TCM and cache](../../intermediate/tcm_and_cache) for more details.

Also, if both CPUs access main RAM, one of the CPUs will have priority over the
other, which will have to wait. ARM7 code and data is normally placed in WRAM so
that the ARM7 and ARM9 aren't competing for main RAM access all the time.
However, there are many situations in which the ARM7 will need to access main
RAM (like when the ARM9 shares a data buffer with the ARM7). The ARM9 cache can
also help in this kind of situations, as the ARM9 will sometimes be able to run
from cached code and modify cached variables while the ARM7 accesses main RAM.

The idea of CPU caches is that they make your code faster because of the
principles of spatial and temporal locality:

- Spatial locality: In general, programs tend to access data that is close to
  the previously accessed data. For example, lots of loops access all elements
  of an array. Loading multiple elements of the array at the same time means
  that the first time the CPU reads an element it will be slow, but the
  following elements will be read much faster.

- Temporal locality: In general, programs tend to access data that has recently
  been used. For example, the coordinates of the "player" object are likely to
  be read by the input handling code, the rendering code, the physics code... If
  you keep the coordinates in the cache all the time the CPU won't have to read
  them from main RAM whenever they are needed.

Statistically speaking, both things are true in all programs. However, there can
be situations in which the application organizes code and data in such a way
that the cache can't work as well as it could. You should always keep in mind
the following ideas:

- The size of a cache line is 32 bytes. Whenever the ARM9 reads from main RAM it
  loads a full cache line to the data or instruction cache as needed. Whenever a
  cache line must be replaced by a new one, the full 32 bytes are written back
  to main RAM (if they have been modified in the cache).

- The size of the data cache is 4 KiB. If you have complicated code that uses
  several data structures, if you can organize them to fit in 4 KiB the code
  will run faster.

- Small functions can be faster than large functions because of the time they
  take to be loaded to the instruction cache.

- The size of the instruction cache is 8 KiB. If you have a very CPU-intensive
  part of your program that fits in 8 KiB, it will be faster than if it doesn't
  fit and the CPU needs to load different parts of it to the cache at different
  times. This means that sometimes `-Os` can be faster than `-O3`, because `-O3`
  can make your code larger.

- The compiler aligns variables and structs to the size defined by the C/C++
  standards. Sometimes it can be useful to align your data to the size of a
  cache line with `ALIGN(CACHE_LINE_SIZE)` to optimize things, but the effect is
  very small. In many cases you would only save one cache line load.

If you consider DTCM (16 KiB) and ITCM (32 KiB) and the data and instruction
caches, the ARM9 has quite a bit of fast memory that it can use outside of main
RAM!

Some examples of how to optimize code to work well with the cache:

- Using a LUT (Lookup table) to calculate things is much faster if you use the
  same LUT for many things at the same time.

  For example, if you have 100 particles moving around and you need to calculate
  the sine and cosine of all of their angles, using a LUT can be very helpful.
  If you access the same LUT 100 times for sine and cosine the first few values
  will be read slowly because nothing in the table is in the data cache, but
  accesses will become faster and faster as more parts of the LUT are loaded to
  the cache in the loop.

  A common technique for sine and cosine LUTs is that they can be merged into a
  single table (they are the same function displaced by an offset). Most of the
  time you will check the sine and cosine of an angle at the same time, so the
  loop of the example will actually access the same LUT 200 times, not 100.

- In general, it's better to have arrays of structs than multiple arrays (one
  for each field in the struct), but not always. This happens because normally
  you will access multiple fields of each element of the array, not just one.

  However, think of a scenario like having a pool of 100 possible objects that
  can appear on the screen at the same time, each one of them with an "active"
  field. Most of the code accessing the array will consist of loops that check
  if each object is active or not and skip the object if it's disabled. This
  means that each loop will potentially read a full cache line for each element
  of the array when it only really needs to read one bit:

  ```c
  #define MAX_OBJECTS 100

  typedef struct {
      int x, y;
      int other_data[10];
      bool active;
  } object_t;

  object_t object[MAX_OBJECTS];

  for (u32 i = 0; i < MAX_OBJECTS; i++)
  {
      if (!object[i].active)
          continue;

      draw_object(object[i].x, object[i].y);
  }
  ```

  It may be faster to have an array of objects without that "active" field, and
  to have a separate array of integers in which each bit of an integer
  represents the "active" field of a different object:

  ```c
  #define MAX_OBJECTS 100

  typedef struct {
      int x, y;
      int other_data[10];
  } object_t;

  object_t object[MAX_OBJECTS];
  u32 object_active[(MAX_OBJECTS / sizeof(u32)) + 1];

  for (u32 i = 0; i < MAX_OBJECTS; i++)
  {
      if ((object_active[i / 32] & (1 << (i & 31))) == 0)
          continue;

      draw_object(object[i].x, object[i].y);
  }
  ```

  With this approach, each cache line will hold the "active" field of 32 * 8 =
  128 objects. This may be enough to offset the slower code that checks the bit,
  but only if there are many objects that aren't active! If all objects are
  active, you will need to load to the cache all the data anyway, so it could
  even be slower. Remember to profile your code when trying optimizations like
  this one.
