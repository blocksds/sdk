---
title: How to debug programs
weight: 10
---

While developing applications with BlocksDS you will reach situations where you
need to debug your code. There are different ways to do this with BlocksDS, and
they are described below.

## 1. Regular text console

printf-debugging is the most basic way to debug code, but it's still very useful
so it's worth mentioning it.

It's very easy to setup libnds so that the output of `printf()` is sent to a
text console on the screen. All you have to do is:

```c
consoleDemoInit();

printf("This is a debug message: %d", 123);
```

By default, all text sent to `stdout` and `stderr` will be directed to this
console after it is initialized.

Check [this example](https://github.com/blocksds/sdk/tree/master/examples/console/ansi_console)
to see it in action.

Note that if you initialize the console like this, it will setup the sub screen
to only display the text console, which can be very limiting if your application
is already using that screen for something else. You can use `consoleInit()` to
initialize the console in a more specialized way. Check the examples to see how
to use this function.

You can check [this example](https://github.com/blocksds/sdk/tree/master/examples/graphics_3d/text_over_3d)
for a more advanced setup that allows you to display text over a 3D scene. You
can set it up over other 2D layers as well.

## 2. no$gba-style console

If you don't have any available memory or background layer to display the text
console, it's possible to setup libnds so that `stderr` is redirected to the
debug console of no$gba (debugger version) instead of a text console on the
screen. It also works in melonDS.

This has the advantage of being less invasive for your application, and
it will let you copy-paste text in case you want to use that for something.

In no$gba, you can see the messages this "Window" > "TTY Debug Messages". You
can switch between messages sent by the ARM7 and ARM9 by changing the selected
CPU in the debugger with "Window" > "ARM9 (Main CPU)" and "Window" > "ARM7 (Sub
CPU)".

For melonDS you need to launch the emulator from the command line with a command
like `melonDS your_rom.nds`. This way, melonDS will print all messages to the
console you launched the emulator from. You can't switch between CPUs, so all
messages are printed to the same console (you will need to add a way to identify
the messages in your code if they can happen from both CPUs).

To use it, follow the instructions below.

In the ARM7 the default behaviour is to send `stderr` messages to this console,
so you don't need to do initialize anything. In the ARM9 you need to enable it
by doing this:

```c
// Redirect stderr to the no$gba debug console. By default both stdout and
// stderr are redirected to the libnds console.
consoleDebugInit(DebugDevice_NOCASH);
```

After that, you can send messages to the console like this (it will
automatically insert a new line after the message):
```
fprintf(stderr, "Debug message!");
```

Remember to check the [example](https://github.com/blocksds/sdk/tree/master/examples/debug/nocash_debug)
to see how to do it in a real application.

Note that the string can contain special parameters that will be filled in by
the emulator:

The ascii string may contain parameters, defined as `%param%`:

- `r0`, `r1`, ... `r15`: Show register value as a hexadecimal number.
- `sp`, `lr`, `pc`: Same as `r13`, `r14` and `r15`.
- `scanline`: Current scanline number.
- `frame`: Number of frames since boot.
- `totalclks`: Number of clocks since boot.
- `lastclks`: Number of clocks since the last `lastclks` or `zeroclks`.
- `zeroclks`: Reset the `lastclks` counter.

From C you can use them like this (note that you need to use `%%` because `%` by
itself is used to format strings!):
```c
fprintf(stderr, "Frame: %%frame%%\n");
```

More information [here](https://problemkaputt.de/gbatek.htm#debugmessages).

## 4. assert(), sassert() and debug builds of libnds

libnds has a release and a debug build. The difference is whether assertions are
enabled or not.

libnds has a function called `sassert()`, which is similar to `assert()` from
libc. It uses it internally in many places to detect problems like setting up
backgrounds with invalid parameters, or using values outside of their valid
ranges.

Also, it is possible to use the standard `assert()` function of libc. If the
expression fails on the ARM9, it will stop the program and print an error
message on the screen. On the ARM7 it will print a message to the no$gba/melonDS
console and stop execution.

It's always a good idea to use this function in your code to warn the developer
about problems that need to be fixed, but that can't happen during normal
execution of the program. For errors that may happen during normal execution of
the program (like running out of RAM or VRAM) you shouldn't use `assert()`. You
should never check the return value of `malloc()` with it, for example. You can
use it to check that the background layer you are using is between 0 and 3.

The release build of libnds doesn't include any assertion, the debug build does.
However, lots of `sassert()` calls are present in headers of libnds, and they
will be compiled in or not depending on the build setup of your program.

In order to do a proper release build you need to add `-DNDEBUG` to the `CFLAGS`
of your Makefile, and you need to use `-lnds9` in `LIBS`.

To do a debug build, leave `CFLAGS` without `-DNDEBUG`, and add `-lnds9d` to
`LIBS` instead of `-lnds9`.

You need to do the same in the ARM7 (with `-lnds7` and `-lnds7d` instead).

## 5. No$gba debugger

No$gba debugger version has an impressive debugger that lets you see the
disassembly of the code running on the emulator, the contents of the RAM, the
state of all I/O registers, even the state of VRAM and the commands sent to the
3D GPU and their effect.

This emulator isn't as accurate as melonDS, but it's definitely a great tool to
have in your arsenal. You can download it [here](https://problemkaputt.de/gba.htm).

[This website](https://problemkaputt.de/gbapics.htm) has lots of screenshots of
the debugger so that you can get an idea of what it is capable of.

## 6. Exception handler

libnds comes with an exception handler that can help you debug some crashes in
your program. It appears as a red screen that says "Guru Meditation Error!"
followed by a lot of information:

![Exception handler](../exception_handler.png "Exception handler")

You can enable it by calling this function, preferably right at the start of
`main()`:

```c
defaultExceptionHandler();
```

The exception handler is used whenever:

- The CPU tries to execute an `undefined instruction`.

  This usually means that the CPU is executing memory that isn't code, which can
  mean anything from memory corruption to a function pointer that was invalid.

  libnds sometimes deliberately executes an undefined instruction to crash in a
  controlled way.

- The CPU tries to access memory that is protected (`data abort`).

  This happens because libnds setups the MPU (Memory Protection Unit) in a way
  that only lets the CPU access addresses where there is actual memory. The
  32-bit address space of the CPUs is too big for all the memory available on
  the NDS, there are large gaps between different memory regions. Accessing
  the gaps will cause a data abort. Also, the MPU has been setup to cause data
  aborts if the program tries to write to read-only memory (like the BIOS).

  Note that on a DSi the GBA slot memory isn't accessible. This may cause
  programs to crash if they haven't been tested on a DSi before. You can solve
  this issue by checking `isDSiMode()` before accessing the GBA slot memory.

  Normally this error means that you have a pointer that doesn't point to valid
  memory and you have tried to access it.

Check [this example](https://github.com/blocksds/sdk/tree/master/examples/debug/exception_handler)
to see it in action. Note that this will only work in melonDS and on hardware.
Other emulators won't emulate it at all.

### 6.1 How to use all the information on the screen?

The most important thing on the screen are the values of `pc` and `addr` at the
top of the screen. `pc` says what's the address where the CPU crashed. You can
translate this to an address in your code by running `addr2line` in your
terminal. For example:

```sh
/opt/wonderful/toolchain/gcc-arm-none-eabi/bin/arm-none-eabi-addr2line \
    -e build/debug_exception_handler.elf -a 0x2001252
```

Another option (that can be more useful!) is to use `objdump` to generate a text
file with the assembly code of your program. The templates and examples have a
Makefile target to do this for you. For ARM9-only projects, run:
```sh
make dump
```

For combined ARM7+ARM9 projects, run:
```sh
make -f Makefile.arm9 dump
make -f Makefile.arm7 dump
```

With this, all you have to do is to open the resulting `dump` file in a text
editor and look for the address on the screen.

Also, if you understand assembly language, you can try to use the rest of the
information on the screen to try to understand what the code was doing when it
crashed.

## 7. GDB with melonDS

melonDS has a GDB stub, so you can connect to it and debug your programs with
any command line or GUI tool that supports the GDB protocol

This section needs to be expanded in the future to add more information of how
to use it.
