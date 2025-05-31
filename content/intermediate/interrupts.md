---
title: Interrupts
weight: 0
---

## 1. Introduction

The DS has many subsystems (video, audio, timers, etc), and they all generate
events that can be setup to interrupt the execution of code. For example, you
can setup a countdown timer so that when it reaches 0 the ARM9 stops doing
whatever it was doing, it jumps to a special routine created by you, and it
returns to the previous code after running that routine.

Of course, all you need to do is understand what kind of interrupts there are,
and how to assign your interrupt handling routines so that libnds uses them.
This is very easy, and you've been using interrupts already in a simplified way!

Previous chapters of this tutorial have used `swiWaitForVBlank()` to synchronize
the main loop with the screen refresh. Internally, libnds enables the vertical
blanking interrupt. This is an interrupt that happens whenever the screen has
finished drawing a frame. `swiWaitForVBlank()` simply waits for this to happen
and then it returns to your code.

Let's get into more interesting examples!

## 2. Basic example

We're going to setup an interrupt handler to be run when the screen has finished
being drawn and the vertical blanking period starts.

First, we need to understand how the screen is drawn. Every frame the hardware
starts sending pixels in horizontal lines from the top left. When the line is
complete, the screen controller enters "horizontal blanking period", in which no
pixels are sent to the screen. You're free to edit video settings during this
period. After a bit, the next line starts to be drawn. This process is repeated
until the last line is drawn, and then the screen controller enters "vertical
blanking period". This period lasts the same amount of time as several
horizontal lines. You can read a bit more about this
[here](https://gbadev.net/tonc/video.html#sec-blanks). You can read the timings
for a Nintendo DS in [GBATEK](https://problemkaputt.de/gbatek.htm#dsvideostuff).

This section is based on [`examples/interrupts/vbl_interrupt`](https://github.com/blocksds/sdk/tree/master/examples/interrupts/vbl_interrupt).

Note that the VBL interrupt is special because libnds enables it even before
`main()` is reached. We will see an example of another interrupt in the next
section. However, libnds already calls `irqEnable(IRQ_VBLANK)` before reaching
`main()`, so you don't need to do it.

```c
int irq_frame_count;

void vbl_handler(void)
{
    irq_frame_count++;
}

int main(int argc, char **argv)
{
    consoleDemoInit();
    irqSet(IRQ_VBLANK, vbl_handler);

    while (1)
    {
        swiWaitForVBlank();
        printf("%d\n", irq_frame_count);
    }
}
```

As you can see, all you need to do is call `irqSet()` from your code, specify an
interrupt handler, and say which interrupt type is the one you're interested in.
Each interrupt type can have one handler, and you can disable the handler by
calling `irqSet()` with a `NULL` interrupt handler.

In this example `vbl_handler()` will get called whenever the VBL interrupt
happens, interrupting the main loop. It will increase the counter and return to
the main loop. It will also allow `swiWaitForVBlank()` to finish.

Note: You don't need to worry about it normally, but the ARM7 requires special
handling. You must call `irqInit()` in your `main()` function if you want to use
interrupts. The default ARM7 cores do it almost as soon as `main()` starts.

## 3. Hardware timers

Now we're going to see a more interesting example that involves hardware timers.

The DS has 4 hardware timers per CPU (4 for the ARM9, other 4 for the ARM7). The
timers can be setup in different modes (count up, count down) and they can even
be chained.

Most times you don't need the accuracy of a hardware timer. Your game loop can
be updated at the same rate as the screen refresh (using `swiWaitForVBlank()`)
and most other things have their own interrupts that happen at specific events
that require your attention. However, there are cases in which having an
accurate timer may be helpful. For example, if you're creating a music player,
you will need to use hardware timers to keep track of the audio that has payed
so far and how much you need to decode.

This section is based on [`examples/time/timers`](https://github.com/blocksds/sdk/tree/master/examples/time/timers).

```c
void timer0_handler(void)
{
    timer0_count++;
}

int main(int argc, char **argv)
{
    ...

    // Timer 0 will be called every second. This is a long time to wait, so a
    // big divider is required so that the timer counter can fit enough ticks to
    // count up to a second.
    timerStart(0, ClockDivider_1024, timerFreqToTicks_1024(1), timer0_handler);

    ...
}
```

This timer is setup to trigger an interrupt every second.

In this case, `timerStart()` sees that you're providing an IRQ handler
(`timer0_handler()`) and it calls `irqSet(IRQ_TIMER(0), timer0_handler)` and
`irqEnable(IRQ_TIMER(0))` internally.

The clock divider is the accuracy of the timer. The timer count value is a
16-bit value, which isn't that much. It can increase at different frequencies
that use the bus clock frequency (~33 MHz) as base, and then it's divided by a
divider that can be 1, 64, 256 or 1024. This means that the counter can increase
at a rate of 33 / 1 MHz, 33 / 64 MHz, 33 / 256 MHz, or 33 / 1024 MHz.

If you want the highest accuracy allowed by the hardware, you will need to use a
small divider, but you won't be able to count for a long time. If you want very
low accuracy (if you're counting seconds, for example), you can set a very high
divider, and you will be able to count for a long time.

The code is setting up a timer that has to cause an interrupt every second, so
we don't need a lot of accuracy. We can use a `ClockDivider_1024` value, and
then use `timerFreqToTicks_1024(1)` to generate the raw value that sets up the
timer to overflow once per second.

This other example is using a clock divider of 256 to setup a timer that fires
five times per second:

```c
void timer1_handler(void)
{
    timer1_count++;
}

int main(int argc, char **argv)
{
    ...

    // Timer 1 will be called five times per second. The total number of ticks
    // is smaller, so a smaller divider can be used.
    timerStart(1, ClockDivider_256, timerFreqToTicks_256(5), timer1_handler);

    ...
}

```

## 4. FPS counter

Now that we know how to setup a timer we can apply this to calculating the exact
FPS of an application.

This section is based on [`examples/interrupts/fps_counter`](https://github.com/blocksds/sdk/tree/master/examples/interrupts/fps_counter).

The idea behind a FPS counter is that you need a reliable way to measure time (a
hardware timer) because the main loop of your application may take longer than
expected to finish, so it may not be executed at a predictable frequency.

Essentially, we increase a counter in our main loop. This will give us the
number of frames that are actually drawn as they get drawn, even if some of them
are delayed.

Then, we setup a hardware timer to call a handler once per second. This function
is called exactly once per second, it can't be delayed. We can take a look at
how many frames have been drawn until that point and save it in a `fps`
variable:

```c
int fps;
int irq_frame_count;

void timer0_handler(void)
{
    fps = irq_frame_count;
    irq_frame_count = 0;
}

int main(int argc, char **argv)
{
    // Timer 0 will be called every second
    timerStart(0, ClockDivider_1024, timerFreqToTicks_1024(1), timer0_handler);

    consoleDemoInit();

    while (1)
    {
        irq_frame_count++;
        swiWaitForVBlank();

        printf("FPS: %d  ", fps);
    }
}
```

## 5. Nested interrupts

TODO: Section in progress
