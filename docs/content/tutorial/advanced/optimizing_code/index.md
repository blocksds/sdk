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

{{< callout type="error" >}}
This chapter is a work in progress...
{{< /callout >}}
