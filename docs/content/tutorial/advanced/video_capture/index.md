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

Before learning how to use the video capture component, we need to understand
how the additional video modes work.

The sub graphics engine doesn't support 3D, video capture or the special video
display modes.

{{< callout type="error" >}}
This chapter is a work in progress...
{{< /callout >}}
