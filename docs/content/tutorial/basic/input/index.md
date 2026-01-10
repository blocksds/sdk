---
title: User input
weight: 2
---

## 1. Introduction

The first thing we're going to learn is how to read the buttons and touch
screen. It's actually a very easy thing to do, and it will help us in all
future chapters as we will be able to demonstrate things in a more interesting
way (for example, we will be able to use the direction pad to move things
around!).

Note that most of the code that we see runs on the ARM9 CPU of the DS. The ARM7
CPU can be programmed, but BlocksDS comes with a default build of the ARM7 that
can be used if you don't need anything special. We will use the default ARM7
binaries in most chapters of this tutorial.

## 2. Buttons

The DS has the following buttons: L, R, A, B, X, Y, D-Pad, Start, Select.
However, there are also two other buttons that you may not have thought about
initially: the touch screen and the lid. There is also a "debug" button that
official development consoles have, and some emulators support. And, finally,
DSi consoles have a "power" button that can be read from code (but not the DS!).
The power button is a bit special, so we will leave it for later.

Let's see an example of a very simple demo that can read the state of the
buttons:

```c
// Include for printf()
#include <stdio.h>

// Include for libnds
#include <nds.h>

int main(int argc, char **argv)
{
    // Initialize a basic text console
    consoleDemoInit();

    // Infinite loop
    while (1)
    {
        // Synchronize loop with the new frame. The DS renders at 60 frames per
        // second, so this loop will run 60 times per second.
        swiWaitForVBlank();

        // Refresh touchscreen and key state in libnds
        scanKeys();

        // Get list of all the keys that are currently pressed
        unsigned int keys_held = keysHeld();

        // Clear the text console
        consoleClear();

        // Print state of all the keys
        printf("Keys: %X\n", keys_held);

        // Print message if A is pressed
        if (keys_held & KEY_A)
            printf("A is pressed\n");

        // Print message if up and B are pressed at the same time
        if ((keys_held & KEY_B) && (keys_held & KEY_UP)
            printf("Combination is pressed\n");
    }
}
```

As you can see, this example already does some interesting things.

`consoleDemoInit()` is a pretty useful function for prototyping code. It
initializes a text console in the sub screen so that you can print some log
messages, etc. We will learn the differences between the main and the sub
screens in another chapter. `consoleClear()` cleans the screen used for the
console, and `printf()` prints text to that console.

`swiWaitForVBlank()` is something you will see in almost every program. The
screen of the DS works by drawing pixels in rows. It draws 256 pixels per rows,
up to 192 lines. This period is called "vertical draw". When it reaches line 192
it enters "vertical blanking" period, in which it doesn't draw anything for the
time equivalent to drawing 71 more lines. We will learn more about this later.
For now, just know that `swiWaitForVBlank()` waits until the next vertical
blanking period. The code of this demo takes much less time to finish than a
full frame, so calling this function will synchronize each iteration of the loop
with a new frame.

`scanKeys()` reads the hardware registers related to the keys and the touch
screen and saves its current status in libnds. `keysHeld()` returns a bit mask
of all the keys currently pressed (for example, if A and B are pressed it will
return `KEY_A | KEY_B`).

There are two more functions that return the key state. `keysDown()` returns the
keys that have been pressed in this frame but weren't pressed the previous
frame. They are returned by `keysDown()` only for one frame. `keysUp()` returns
the keys that have just been released, for one frame only. In short: when you
press one key, `keysDown()` and `keysHeld()` return 1 for one frame. Then,
`keysDown()` will start returning 0 while `keysHeld()` stays as 1. Eventually,
when you release the key, `keysHeld()` becomes 0, and `keysUp()` becomes 1 for
one frame.

Something interesting you may want to know is that the ARM9 CPU doesn't have
access to the values of the keys X and Y or the touch screen. libnds has code
that sends this information to the ARM9 so that you don't have to do it.

You can check [`examples/input/key_input`](https://codeberg.org/blocksds/sdk/src/branch/master/examples/input/key_input)
for a more interesting example.

## 3. Touch screen

This is one of the most interesting features of the Nintendo DS family of
consoles, so it's natural you want to use it. libnds reads the firmware
callibration settings and takes them into consideration when checking where the
player has pressed the touch screen so that applications don't need to worry
about it.

```c
#include <stdio.h>
#include <nds.h>

int main(int argc, char **argv)
{
    consoleDemoInit();

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();
        unsigned int keys_held = keysHeld();

        consoleClear();

        // Only handle touch screen presses if it is pressed
        if (keys_held & KEY_TOUCH)
        {
            printf("Touchscreen pressed\n");

            // Structure that holds information about the touch screen
            touchPosition touch_pos;

            // Read touch screen. This function will only return useful data
            // when keysHeld() says that KEY_TOUCH is pressed.
            touchRead(&touch_pos);

            // Print the coordinates that are currently pressed. It will return
            // values in pixels (X from 0 to 255, Y from 0 to 191).
            printf("Coordinates: %d, %d\n", touch_pos.px, touch_pos.py);
        }
    }
}
```

You can check [`examples/input/touch_input`](https://codeberg.org/blocksds/sdk/src/branch/master/examples/input/touch_input)
for a more interesting example.

It's also possible to know the pressure applied to the touch screen. You can
check [`examples/input/touch_pressure`](https://codeberg.org/blocksds/sdk/src/branch/master/examples/input/touch_pressure)
to see how to do it. However, the values returned by it depend on the DS model,
and they are more accurate in DS than in DSi or 3DS.

If you want an example of how to do gesture recognition you can check
[`examples/input/gesture_recognition`](https://codeberg.org/blocksds/sdk/src/branch/master/examples/input/gesture_recognition).
This is an advanced topic, so don't check the example if you're just getting
started.

## 5. Closing the lid

When the Nintendo DS is closed while running a game, it enters sleep mode. This
isn't an automatic process, it must be handled by software. However, libnds
already handles it for you.

When the lid is closed for more than a few frames libnds will enter sleep mode
automatically. This means that your code can detect whether `KEY_LID` is pressed
or not for a few frames, but you don't have to do anything for the DS to enter
sleep mode.

## 6. Power button

The DSi has a power button that can be read from software. Normally you don't
need to do anything about this because libnds takes care of it. The ARM9 doesn't
even have access to it. An ARM7 binary calls `setPowerButtonCB()` to setup a
callback that will be called whenever the power button is pressed. The default
ARM7 binaries of BlocksDS will simply exit `main()` when the power button is
pressed, which should trigger a soft reset of the console.

You can see what the default ARM7 binary does in
[this link](https://codeberg.org/blocksds/sdk/src/branch/master/sys/arm7/main_core/source/main.c).

## 7. Debug button

This is just a button pressent in development Nintendo DS units that can't be
found in retail units. You can safely ignore its existence, but some emulators
let you map it to a key if you want to use it. In that case, use `KEY_DEBUG` to
detect whether it's pressed or not.
