---
title: Audio
weight: 60
---

## 1. Introduction

The DS has 16 sound channels that are mixed by hardware, so you can play up to
16 simultaneous sounds without having to dedicate any CPU time. The playback
frequency, volume and panning can be changed for each channel independently.
This is a pretty flexible system that can be used to play music and sound
effects simultaneously while dedicating most of the CPU time to the game logic
instead of mixing sounds.

Something important to consider is that the ARM9 doesn't have access to the
sound hardware, only the ARM7 does. Normally you don't need to worry about it
because libnds provides some helpers that can communicate with the default ARM7
executable to tell it to play sounds. The functions are designed to provide a
simple interface to play sound effects, but it isn't very useful if you want to
play music. For that, you need to use other libraries.

BlocksDS comes Maxmod and LibXM7, two libraries that can play music on the DS.

- **Maxmod**: It's a library that supports playing music in formats MOD, XM, S3M
  and IT, and sound effects in WAV format. In fact, if you use Maxmod, you
  generally don't need to use the helpers of libnds to play sound effects. You
  can use Maxmod to play music and audio effects. It requires all audio files to
  be converted and packaged as a "sound bank" before being played.

- **LibXM7**: This library only supports playing MOD and XM files, and it
  doesn't have support to play sound effects. However, it doesn't require
  converting songs to any format before playing them, which can be useful in
  some situations.

Most of the time you'll be using Maxmod for everything, but you can definitely
replace it by using LibXM7 for music and libnds for sound effects.

## 2. Maxmod

This is the most common option. Maxmod is a pretty flexible library that lets
you combine the hardware-mixed sound channels and software-mixed channels in
case you require more than 16 simultaneous sounds. This library is used by
default in BlocksDS projects, so you don't need to do anything special to use it
in your projects.

There are a few examples of Maxmod in the BlocksDS repository. This section is
based on the following example:
[`examples/maxmod/basic_sounds`](https://github.com/blocksds/sdk/tree/master/examples/maxmod/basic_sounds)

To use this library you just have to add your music files to a folder (for
example, "audio") and add the folder to `AUDIODIRS` in the Makefile. The build
system will look for MOD, XM, S3M, IT and WAV files in this folder and create a
soundbank with it. You will need to include the soundbank and its definitions in
your application:

```c
#include <maxmod9.h>        // Maxmod header for the ARM9
#include <nds.h>

#include "soundbank.h"      // Header with IDs of songs and sound effects
#include "soundbank_bin.h"  // Pointer to the soundbank itself
```

In the example there are three files in the "audio" folder:
`fire_explosion.wav`, `lasse_haen_pyykit.xm` and `parallax_80599.xm`. This means
that `soundbank.h` will contain three definitions based on their file names. For
example:

```c
#define MOD_PARALLAX_80599    0
#define MOD_LASSE_HAEN_PYYKIT 1
#define SFX_FIRE_EXPLOSION    20
```

To use your songs and sound effects, first you need to initialize audio with
`soundEnable()`, which is found in libnds (you can also disable it with
`soundDisable()`, but this isn't normally required).

You can initialize Maxmod with:

```c
mmInitDefaultMem((mm_addr)soundbank_bin);
```

After the library is initialized you need to load specific songs and sound
effects before you can use them. For example:

```c
mmLoad(MOD_PARALLAX_80599);
mmLoad(MOD_LASSE_HAEN_PYYKIT);

mmLoadEffect(SFX_FIRE_EXPLOSION);
```

Now, you can play songs with `mmStart(MOD_LASSE_HAEN_PYYKIT, MM_PLAY_LOOP)`, for
example, and you can stop them with `mmStop()`.

You can play sound effects with `mmEffect(SFX_FIRE_EXPLOSION)`, for example.
That function returns a `mm_sfxhand` value that can be passed to other functions
to modify the effect (like `mmEffectVolume()` and `mmEffectPanning()`) and to
`mmEffectCancel()` to stop it before it finishes.

If you want to play sound effects in a loop you will need to save your WAV file
with looping information. This isn't a very well-known feature of WAV files, but
they can be saved with embedded loop data (RIFF chunk type `smpl`), which
contains the starting point and end of the part that loops.

Check the [documentation of Maxmod](https://blocksds.skylyrac.net/docs/maxmod/index.html)
and the [examples](https://github.com/blocksds/sdk/tree/master/examples/maxmod)
for more information.

## 3. LibXM7

LibXM7 is less powerful than Maxmod, but it has some very nice features that
make it a serious alternative to Maxmod.

TODO: This section is a work in progress!

Check the [documentation of LibXM7](https://blocksds.skylyrac.net/docs/libxm7/index.html)
and the [examples](https://github.com/blocksds/sdk/tree/master/examples/libxm7)
for more information.

## 4. Sound helpers of libnds

The sound helpers of libnds allow you to use all of the features provided by the
hardware channels of the DS. All channels (0 to 15) support playing PCM formats
(regular waveforms, like WAV files). In addition, channels 8 to 13 support
playing PSG (programmable sound generator) rectangular waves of any frequency,
and channels 14 and 15 can play white noise of any frequency. In most cases, PSG
and white noise is never used because PCM is simply more flexible and convenient
from a development perspective:

- Playing rectangular waves or white noise uses a sound channel, just as playing
  a PCM sound.

- For the person creating the audio of a game it's far easier to save audio as a
  PCM file than to edit code or configuration files trying to define which
  frequency to use, etc.

- PCM files can play any kind of sound, not just rectangular waves or white
  noise. It's easier to make games sound well with PCM files.

The main disadvantage of PCM sound effects is that they need more ROM space.
However, this is generally fine because NDS ROMs can be very big (the biggest
official game cartridges are 512 MB).

Now, let's see how to play sounds of all types. Check the following example:
[`examples/audio/play_sounds`](https://github.com/blocksds/sdk/tree/master/examples/audio/play_sounds)

The audio sample in this example has been created with **Audacity**. You can
open any sound effect with it and export it as "Uncompressed" format, without
header, with signed 16-bit values. You can also choose to export it with
unsigned 8-bit values. Any program that can export to this format will work.
**sox** is a command line tool that can also be used to convert audio files to
this format.

Copy all your audio files to the "data" folder in your project and add it to
`BINDIRS` in your Makefile. You can have as many folders as you want in
`BINDIRS`, so you can have a folder called "sfx" instead, for example. The main
thing to remember is to rename the files to have the ".bin" extension.

In the code of your application, the first thing you need to do is to initialize
sound with `soundEnable()`. Then you can start playing sounds. There are two
options:

- You can let libnds look for any unused sound channel and play the sound there.
  For that, you can use `soundPlaySample()`, `soundPlayPSG()` and
  `soundPlayNoise()`. The functions return the channel that libnds has picked
  for the sound effect in case you want to change its volume, frequency,
  panning, or stop it. They return -1 if they can't find any free channel.

- You can select the channel you want to use. For that, use
  `soundPlaySampleChannel()`, `soundPlayPSGChannel()` and
  `soundPlayNoiseChannel()`. The functions expect you to pass a valid channel to
  be used for that sound effect, and they replace whatever sound was being
  played in that channel. This can be useful if you're using LibXM7 for music
  and you need to hardcode channel numbers for sound effects.

This is how you can use the functions:

```c
#include <nds.h>

// This file is generated from "nature_raw.bin"
#include "nature_raw_bin.h"

int main(int argc, char **argv)
{
    // Make sure that the sound engine on the ARM7 is initialized
    soundEnable();

    // Variable to hold the channel being used by the PCM sound
    int ch_wav = -1;

    // In the case of the PSG and noise, we're going to hardcode the channel number
    const int ch_psg = 12;
    const int ch_noise = 14;

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint16_t keys_down = keysDown();

        if (keys_down & KEY_Y)
        {
            // If the sample sound is active, stop it first
            if (ch_wav != -1)
                soundKill(ch_wav);

            ch_wav = soundPlaySample(nature_raw_bin, // Pointer to sample
                        SoundFormat_16Bit,           // Format: Signed 16 bits
                        nature_raw_bin_size,         // Size in bytes of the sample
                        11025, // Frequency in Hz
                        127,   // Volume: Max
                        64,    // Panning: Center
                        false, // Disable looping
                        0);    // Loop start point (not used here)
        }

        if (keys_down & KEY_A)
        {
            // Play a rectangular wave with 50% duty cycle (50% on and 50% off).
            // Replace the previous sound.
            soundPlayPSGChannel(
                    ch_psg              // Channel to use
                    DutyCycle_50,       // Duty cycle
                    440 + rand() % 256, // Random freq between 440 and 696 Hz
                    127,                // Volume: Max
                    64);                // Panning: Center
        }

        if (keys_down & KEY_X)
        {
            // Play noise in a specific channel. Rplace the previous sound.
            soundPlayNoiseChannel(
                    ch_noise,           // Channel to use
                    700 + rand() % 256, // Random freq between 700 and 956 Hz
                    127,                // Volume: Max
                    64);                // Panning: Center
        }

        if (keys_down & KEY_B)
        {
            // Stop any sound that is active

            if (ch_wav != -1)
                soundKill(ch_wav);
            ch_wav = -1;

            soundKill(ch_psg);
            soundKill(ch_noise);
        }

        if (keys_down & KEY_START)
            break;
    }

    // Disable sound when we're leaving the application
    soundDisable();

    return 0;
}
```

You can use functions like `soundSetVolume()`, `soundSetFreq()` or
`soundSetPan()` to adjust a sound currently being played, or `soundPause()` and
`soundResume()` to pause and unpause a sound.
