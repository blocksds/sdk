---
title: Video capture
weight: 4
---

## 1. Introduction

The main video engine of the DS has a very flexible video capture system. It
allows you to capture either the 3D output of the GPU or the full video output
of the main video engine and save it to one of the main VRAM banks (A, B, C or
D). This output can then be processed by the application or displayed right
away.

This chapter will explain how this system works in detail and it will show a few
examples of things that can be done with it (like displaying 3D graphics on both
screens).

## 2. Video System Diagram

First, we need to understand how the DS hardware works. This diagram (inspired
by the one in [GBATEK](https://problemkaputt.de/gbatek.htm#dsvideodisplaysystemblockdiagram))
shows how the components of the main video engine work together:

![Video system diagram](ds_video_diagram.png)

Note that the sub graphics engine doesn't support 3D, video capture or the
special video display modes.

The two most important settings of this diagram are:

- The switch to select BG0 or the 3D output with `DISPCNT.3`. Applications that
  display 3D graphics in one screen normally display the 3D output by replacing
  BG0 by the 3D output.

- The global video mode selection switch, selected with `DISPCNT.16-17`, lets
  you choose between displaying:

  - The combined output of the 2D and 3D graphics engine. This is the setting
    we've used in the tutorial until now.
  - The contents of VRAM banks A, B, C or D as a 16-bit background.
  - Data stored in main RAM transferred to the display with a special DMA copy.

There are some advanced ways to do video capture, like blending a user-provided
framebuffer with the output of the video engines. We will start with easier
examples that don't combine multiple inputs, and see some more advanced examles
later.

However, before learning how to use the video capture system, we need to
understand how the additional video modes work. They are required for some
advanced configurations that involve video capture.

## 3. Special video display modes

Until now we have only used regular video modes with `videoSetMode()`. Modes
like `MODE_0_2D`, `MODE_5_2D` or `MODE_0_3D` are all using the normal display
mode in `DISPCNT.16-17`. Let's see how to use the other two special video modes.

### 3.1 Direct VRAM display

Video modes `MODE_VRAM_A`, `MODE_VRAM_B`, `MODE_VRAM_C` and `MODE_VRAM_D` allow
the video hardware to render the contents of VRAM A, B, C or D directly to the
screen. The VRAM bank used as source of the data must be in LCD mode (for
example, `vramSetBankA(VRAM_A_LCD)`). This VRAM bank is treated as a 16-bit
bitmap.

This mode isn't useful for regular graphics display, and it's only really useful
when used at the same time as the video capture system. We will get into more
details later but, for now, remember that only VRAM banks in LCD mode can be
used as destination of a video capture. Using the direct VRAM display mode lets
you use the same VRAM bank as the destination of the video capture and as a
screen framebuffer at the same time.

To use this mode, all you have to do is this, for example:

```c
vramSetBankB(VRAM_B_LCD);
videoSetMode(MODE_VRAM_B);
```

You can modify the contents of the framebuffer by using the `VRAM_B` define:

```c
volatile uint16_t *fb = VRAM_B;
fb[256 * y + x] = RGB15(31, 0, 0);
```

Check the following example to see it in action:
[`examples/graphics_2d/video_mode_vram`](https://codeberg.org/blocksds/sdk/src/branch/master/examples/graphics_2d/video_mode_vram)

### 3.2 Main RAM FIFO display

This is another special video mode that lets you keep a framebuffer in main RAM
and transfer it to the video engine with a special DMA transfer mode. This
special DMA transfer needs to be started every frame. For example, you can do
the following to start a new transfer every frame in the VBL handler:

```c
void vbl_handler(void)
{
    // Stop previous DMA transfer if it's active

    dmaStopSafe(3);

    // Handle double buffering

    // ...

    // Start DMA tranfer

    void *fb = framebuffer[framebuffer_displayed];

    // The source address must be in main RAM.
    REG_DMA_SRC(3) = (uintptr_t)fb;
    // The destination must be REG_DISP_MMEM_FIFO.
    REG_DMA_DEST(3) = (uintptr_t)&REG_DISP_MMEM_FIFO;

    REG_DMA_CR(3) =
        DMA_DISP_FIFO | // Set direct FIFO display mode
        DMA_SRC_INC |   // Increment the source address each time
        DMA_DST_FIX |   // Fix the destination address to REG_DISP_MMEM_FIFO
        DMA_REPEAT |    // Don't stop after the first transfer
        DMA_COPY_WORDS | 4; // Copy 4 words each time (8 pixels)
}

int main(int argc, char *argv[])
{
    videoSetMode(MODE_FIFO);

    irqSet(IRQ_VBLANK, vbl_handler);

    // ...
}
```

This is an example of how to do it in practice:
[`examples/graphics_2d/video_mode_fifo`](https://codeberg.org/blocksds/sdk/src/branch/master/examples/graphics_2d/video_mode_fifo)

It's also possible to use this system to provide the input B of the video
capture system and mix it with the output of the 2D or 3D engines.

## 4. Simple capture

The most basic way to use the video capture system is to capture the display,
save it to VRAM, and display it later. You can keep using the main and sub video
engines as usual.

In this case all we need to do is to capture the output of the 3D and 2D video
engines, so we need to use source A of the video capture system, which lets us
choose what to capture. The steps are:

- Set one of VRAM A, B, C or D to LCD mode. It will be used to store the
  captured image.

  ```c
  vramSetBankC(VRAM_C_LCD);
  ```

- Start the video capture, which will capture the following frame. You can
  capture the 3D otuput with `DCAP_SRC_A_3DONLY` or the combined 3D+2D output
  with `DCAP_SRC_A_COMPOSITED`:

  ```c
  REG_DISPCAPCNT =
      // Destination is VRAM_C
      DCAP_BANK(DCAP_BANK_VRAM_C) |
      // Size = 256x192
      DCAP_SIZE(DCAP_SIZE_256x192) |
      // Capture source A only
      DCAP_MODE(DCAP_MODE_A) |
      // Source A = 3D rendered image
      DCAP_SRC_A(DCAP_SRC_A_3DONLY) |
      // Enable capture
      DCAP_ENABLE;
  ```

You can see this system in action in the following example, which lets you
experiment with the 3D and 3D+2D capture modes:
[`examples/video_capture/simple_capture`](https://codeberg.org/blocksds/sdk/src/branch/master/examples/video_capture/simple_capture)

{{< callout type="tip" >}}
You can capture the 3D output with `DCAP_SRC_A_3DONLY` even if you don't display
it directly in background layer 0. For example, you can use `MODE_0_2D` instead
of `MODE_0_3D` and the video capture system will still be able to capture the 3D
output. However, `DCAP_SRC_A_COMPOSITED` won't capture it!
{{< /callout >}}

Once you have this basic capture system working, you can even save the result to
the SD card as a screenshot! The following example uses lodepng to save the
captured image as a PNG file:
[`examples/video_capture/png_screenshot`](https://codeberg.org/blocksds/sdk/src/branch/master/examples/video_capture/png_screenshot)

{{< callout type="error" >}}
This chapter is a work in progress...
{{< /callout >}}
