---
title: Your first program
weight: 1
---

## 1. Setting up the environment

The first thing we need to do is setup the development environment that we're
going to use. If you haven't installed BlocksDS yet, follow the instructions
[here](https://blocksds.skylyrac.net/docs/setup/options/).

You will also need the examples of BlocksDS. I encourage you to edit the
examples and experiment with them, so I advise you to get a copy of the examples
that you can edit freely.

If you have git, you can clone the following repository:

```sh
git clone https://codeberg.org/blocksds/sdk
```

If you don't have git or you don't want to clone the repository you can also
download it as a [ZIP archive](https://codeberg.org/blocksds/sdk/archive/refs/heads/master.zip).

## 2. Getting a NDS emulator

Most of the time you will use emulators to test your applications and games
while you develop them. The following three emulators are recommended.

- **melonDS**

[melonDS](https://melonds.kuribo64.net/) is the most accurate out of the three,
and under active development, and it's available in all major OSes. It's
strongly encouraged that you use this emulator to test your games. It also
supports debugging with GDB, which can be useful once you start to develop
complicated programs. It has pretty good DSi mode support.

- **no$gba**

[no$gba](https://problemkaputt.de/gba.htm) isn't as accurate as melonDS, but it
has an amazing [integrated debugger](https://problemkaputt.de/gbapics.htm). It's
only available on Windows, but it can be easily run on Linux with
[Wine](https://www.winehq.org/). This integrated debugger can be more useful
than GDB in some cases, so it's definitely a great emulator to have in your
arsenal. It has reasonable DSi mode support.

- **DeSmuMe**

[DeSmuMe](https://desmume.org/) isn't as accurate of no$gba or melonDS, but it's
very widespread and available in many platforms. I encourage you to test
everything with DeSmuMe to maximize the number of people that can enjoy your
creations. It doesn't emulate DSi mode features.

## 3. Testing on hardware

Regardless of how many emulators you have, eventually you'll need to test on
hardware to ensure that everything works as expected. Depending on which model
of NDS you own, there are two options:

- **Slot-1 flashcarts**

Slot-1 flashcarts are cartridges shaped like a regular NDS game that have a
microSD slot where you can copy your NDS ROMs. There are many models, and there
isn't any specific model that can be recommended. All slot-1 flashcarts will
work on any Nintendo DS or DS Lite consoles, but only some models will work on
Nintendo DSi or 3DS consoles. My advice is to search information online to
decide which one to buy.

This option is the best option if you own a Nintendo DS or DS Lite. If you own a
DSi or 3DS, this option can still work, but you won't be able to take advantage
of any of the additional power of the consoles.

- **DSi custom firmware**

If you have a Nintendo DSi or 3DS you can install a custom firmware that lets
you run NDS ROMs stored in the SD slot of the console.

## 4. Building your first NDS program

Now that you have the toolchain installed, the examples downloaded, and some way
to run your NDS ROMs, we are going to try to build a very simple demo to check
that everything works.

If you are using Windows, remember that any command that we use to build NDS
programs will need to be run from the "Wonderful Toolchain Shell", which can be
fount in the Start menu. If you're using Linux, any terminal will work.

Regardless of how you have downloaded the code, open the shell and go to the
folder [`examples/console/custom_fonts`](https://codeberg.org/blocksds/sdk/src/branch/master/examples/console/custom_fonts).

From here, run the following command:

```sh
make
```

If everything goes well, you should see something like this:

```
  GRIT    graphics/charmap_futuristic.png
  GRIT    graphics/anuvverbubbla_8x8.png
  GRIT    graphics/charmap_cellphone.png
  CC      source/main.c
  LD      build/console_custom_font.elf
  NDSTOOL console_custom_font.nds
Nintendo DS rom tool (BlocksDS v1.8.1-blocks)
by Rafael Vuijk, Dave Murphy, Alexei Karpenko
```

Load it in an emulator or in your console, you should get something like this:

![Custom fonts](custom_fonts.png)

Congratulations! Now you can start learning how to create your own games and
applications for the DS.
