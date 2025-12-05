---
title: DMA
weight: 3
---

## 1. Introduction

DMA (Direct Memory Access) is special hardware that has only one purpose: to
copy data. It can be used for fast copies of data (like `memcpy()`, but faster),
but it can also be configured to work in much more interesting ways.

DMA can be setup in special modes where the source address or destination
addresses are fixed. For example, you can define an array of values in main RAM,
use it as source address of the copy, and fix the destination address to an I/O
register. That way, whenever the DMA is triggered, a new value will be copied to
that register. This can be used to create visual effects, for example.

There are 4 DMA channels that can be setup independently, but they all work the
same way. Channel 0 has the highest priority, channel 3 has the lowest
priority.

## 2. Regular copies

DMA is a very common way to copy data from main RAM to VRAM. For example, the
following two function calls do the same copy:

```c
dmaCopy(source, destination, size);
memcpy(destination, source, size);
```

Note that source and destination are in a different order in `dmaCopy()` than in
`memcpy()`. You can use the compiler to warn you about this kind of mistakes if
you use `const` to mark your source pointers. For example:

```c
const void *data_in_ram;
void *destination;
size_t size;

dmaCopy(data_in_ram, destination, size); // Correct

dmaCopy(destination, data_in_ram, size); // Incorrect
```

The second call will cause the compiler to print the following warning:

> warning: passing argument 2 of 'dmaCopy' discards 'const' qualifier from
> pointer target type [-Wdiscarded-qualifiers]

It is very common to use `dmaCopy()` and similar functions to copy graphics data
from RAM to VRAM. Normally, data exported by **grit** and other graphics
conversion tools is marked as `const`, so you will always get this kind of
warning. You should do the same and mark your pointers as `const` when it makes
sense.

## 3. Potential problems with DMA

- The DMA can't access ITCM or DTCM memory, so you can't use it to copy data
  from or to those memory regions. This may confuse you if you have data stored
  in the stack (which is normally in DTCM) and you use DMA to copy it to a
  different location. However, this is a generally easy problem to detect and to
  fix (the copy simply won't happen).

- DMA copies can interrupt the CPU until the copy has finished. Interrupts can't
  be handled while a DMA copy is active in this kind of situations. The only way
  the CPU won't be blocked is if it's running code from ITCM and reading data
  from ITCM and DTCM.

- A bigger problem is the cache. DMA can't access the caches, but the ARM9 uses
  the cache all the time. If you edit an array it will likely stay in the data
  cache for a while. If you try to copy it with DMA right away, you won't be
  able to copy it. This will be discussed in the next section of this chapter.

## 4. DMA and data cache

If you're interested in this topic, there are two great articles written by
Cearn about it:

- [DMA vs ARM9 - fight!](https://web.archive.org/web/20210622053504/https://www.coranac.com/2009/05/dma-vs-arm9-fight/)
- [DMA vs ARM9, round 2 : invalidate considered harmful](https://web.archive.org/web/20210622053550/https://www.coranac.com/2010/03/dma-vs-arm9-round-2/)

The main thing making this difficult is that emulators normally don't emulate
the cache. If your copies work fine in an emulator but they show garbage data
when running on real hardware, it's likely you're having issues with the cache.

If you're using **grit** to convert your graphics and they are being added to
the ARM9 as data (like the examples we have seen in the tutorial so far)
everything is fine. That data is in main RAM from the moment the program starts.
The cache is disabled when the program boots, and all the data of the program is
already present in RAM. The cache is enabled before starting `main()` but at
that point main RAM has the final version of the graphics data, so DMA can
access it.

The problem is that you may want copy data with DMA that hasn't been in main RAM
from the start of the program. For example, you can load graphics data from the
filesystem. You can also create your own graphics at runtime with some
algorithm.

Anything that the ARM9 loads to RAM or edits in RAM is likely to stay in the
data cache for a while (until the data cache is needed for something else).
However, this means that you can't just load data from the filesystem and use
DMA to copy it to VRAM right away. You need to tell the data cache to update the
data in main RAM. This is a very simple example of how to do that:

```c
void *destination;

uint16_t my_custom_sprite[8 * 8];
for (int i = 0; i < sizeof(my_custom_sprite); i++)
    my_custom_sprite[i] = rand();

// "Flush" the data cache that holds data from the specified base address with
// the specified size. This tells the data cache that any data it contains that
// should be written to this memory range needs to be written to that memory
// range right away.
DC_FlushRange(my_custom_sprite, sizeof(my_custom_sprite));

dmaCopy(data_in_ram, destination, size);
```

This is all you need to do to ensure you can copy data to VRAM with DMA safely!

In some cases you don't even need to worry about this. For example, when we
reach the chapter of 3D graphics we will use `glTexImage2D()` to load 3D
textures to VRAM. This function uses DMA internally, but it also flushes the
data cache to make sure that the DMA copy works as expected. You will only have
problems with DMA if you call the DMA copy functions directly.

## 5. Special DMA copies

There are several conditions that can trigger a DMA copy apart from simply
"start now". They are different on the ARM9 and ARM7, and you can check them in
[GBATEK](https://problemkaputt.de/gbatek.htm#dsdmatransfers).

We're going to see a practical example of how to use special modes by using the
HBL start mode on the ARM9. This section of the tutorial is based on the
following example: [`examples/video_effects/hblank_scroll_dma`](https://github.com/blocksds/sdk/tree/master/examples/video_effects/hblank_scroll_dma).

![DMA scroll effect](hblank_effect_dma.png)

As you can see, this example is similar to the one we saw in the interrupts
chapter. The difference is in the code that creates the effect.

Let's jump directly to the DMA configuration. In order to use special DMA modes
we need to use `dmaSetParams()`:

```c
dmaSetParams(0, // Channel
             source,
             destination,
             DMA_SRC_INC | // Autoincrement source after each copy
             DMA_DST_FIX | // Keep destination fixed
             DMA_START_HBL | // Start copy at the start of horizontal blank
             DMA_REPEAT | // Don't stop DMA after the first copy.
             DMA_COPY_HALFWORDS | 1 | // Copy one halfword each time
             DMA_ENABLE);
```

This call sets DMA channel 0 to start at the start of the horizontal blank
period, to copy only one halfword, not keep going (repeat copy) so that it
doesn't stop after the first horizontal blank, and to increment the source
addresses but keep the destination address fixed.

This means that as soon as HBL starts, one single halfword (16 bits) will be
copied from a pre-defined array into the destination (which is set to a hardware
register). The next HBL another halfword will be copied, and so on.

There is no way to tell the DMA channel to stop after the frame is finished, so
we need to stop it and restart it ourselves. For that, we can use the vertical
blanking interrupt to setup DMA after every frame is drawn. There are no HBL
copies triggered during the VBL period, so the actual copies will wait until
after VBL is over.

Important: That there won't be any copy before line 0 starts getting drawn. The
first copy will happen when the HBL period of line 0 happens. At that point line
0 will have been drawn. This means that the VBL handler needs to set the I/O
register value that will be used for line 0, and DMA will copy values for lines
1 to 191:

```c
static int angle_offset = 0;

static uint16_t bg0ofs_table[192];

static void vblank_handler(void)
{
    // Stop the previous DMA copy
    dmaStopSafe(0);

    // Fill table
    for (int i = 0; i < 192; i++)
    {
        int32_t value = sinLerp(degreesToAngle(angle_offset + i + 1)) >> 7;
        bg0ofs_table[i] = value;
    }

    // Set the horizontal scroll for the first line. The first horizontal blank
    // happens after line 0 has been drawn, so we need to set the scroll of line
    // 0 now.
    REG_BG0HOFS = bg0ofs_table[0];

    // Make sure that DMA can see the updated values of the arrays and the
    // updated values don't stay in the data cache.
    DC_FlushRange(bg0ofs_table, sizeof(bg0ofs_table));

    // Restart the DMA copy
    dmaSetParams(0,
                 &bg0ofs_table[1], // Skip first entry (we have just used it)
                 (void *)&REG_BG0HOFS, // Write to horizontal scroll register
                 DMA_SRC_INC | // Autoincrement source after each copy
                 DMA_DST_FIX | // Keep destination fixed
                 DMA_START_HBL | // Start copy at the start of horizontal blank
                 DMA_REPEAT | // Don't stop DMA after the first copy.
                 DMA_COPY_HALFWORDS | 1 | // Copy one halfword each time
                 DMA_ENABLE);
}
```

When you're ready to enable the effect, all you need to do is set this function
as the VBL interrupt handler:

```c
irqSet(IRQ_VBLANK, vblank_handler);
```

Every time a frame ends and the VBL period starts, the handler will stop the
previous DMA copy, it will recalculate the array with scroll values, and it will
start a new DMA copy for the next frame.

The main advantage of using DMA is that it requires a lot less CPU time to
create the effect. Using an interrupt handler to update the effect can take a
long time because the global interrupt handler does a lot of work, and it has to
be called once per scanline. If you use DMA you only pay the interrupt handling
cost once (for the VBL interrupt handler) but every other scanline you only
require a few CPU cycles for the copy to be completed.
