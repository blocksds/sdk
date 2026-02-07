// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

// This example uses LibXM7 to play a MOD file and it will use the audio capture
// hardware to capture the output of the audio mixer, and then it will be
// displayed on the screen.
//
// For this to work, channel 1 and channel 3 must be used as timers to control
// the audio capture frequency. Normally, with a music player, we can't be sure
// of which channels are going to be used. However, in the case of LibXM7, it
// starts using channels from the top. The MOD file in this example has 4
// channels, so only channels 11 to 15 are used by LibXM7.

#include <stdbool.h>
#include <stdio.h>

#include <libxm7.h>
#include <nds.h>

// Title: Essentials Intact
// Author: Nightbeat
// Source: https://modarchive.org/index.php?request=view_by_moduleid&query=49587
#include <nb_essentials_mod_bin.h>

// Assign FIFO_USER_07 channel to LibXM7
#define FIFO_LIBXM7 FIFO_USER_07

void song_start(XM7_ModuleManager_Type *module)
{
    fifoSendValue32(FIFO_LIBXM7, (u32)module);
}

void song_stop(void)
{
    fifoSendValue32(FIFO_LIBXM7, 0);
}

// Captured samples per second.
#define CAPTURE_SAMPLE_RATE 22000

// Set the buffer size to be just enough to record samples for one frame. Then,
// we will retrigger the capture again.
#define CAPTURE_BUFFER_SIZE (((CAPTURE_SAMPLE_RATE / 60) / 4) + 1)

u32 buffer_right[CAPTURE_BUFFER_SIZE];
u32 buffer_left[CAPTURE_BUFFER_SIZE];

void capture_start(void *buffer_right, void *buffer_left)
{
    soundCaptureStart(buffer_left, CAPTURE_BUFFER_SIZE, 0, false, true, false,
                      SoundCaptureFormat_8Bit);
    soundCaptureStart(buffer_right, CAPTURE_BUFFER_SIZE, 1, false, true, false,
                      SoundCaptureFormat_8Bit);

    // The only thing we care about here is the sample rate of the channels 1
    // and 3. Even when the channel is disabled, as long as the frequency is
    // set, audio capture will drive the sound capture circuit.
    soundSetFreq(1, CAPTURE_SAMPLE_RATE);
    soundSetFreq(3, CAPTURE_SAMPLE_RATE);
}

// You can also allocate this with malloc()
static XM7_ModuleManager_Type module;

int main(int argc, char **argv)
{
    // Setup the top screen to display the waveform as a bitmap
    videoSetMode(MODE_5_2D);
    vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
    int bg = bgInit(2, BgType_Bmp16, BgSize_B16_256x256, 0, 0);

    consoleDemoInit();

    printf("Audio capture example\n");
    printf("=====================\n");
    printf("\n");
    printf("LibXM7 is playing a MOD with 4\n");
    printf("channels mixed by hardware.\n");
    printf("\n");
    printf("Captured output:\n");
    printf("  Top:    Left speaker\n");
    printf("  Bottom: Right speaker\n");
    printf("\n");
    printf("START: Return to loader\n");

    u16 res = XM7_LoadMOD(&module, nb_essentials_mod_bin);
    if (res != 0)
    {
        printf("LibXM7 error: 0x%04x\n", res);
        while (1)
            swiWaitForVBlank();
    }

    // We need to ensure that the audio capture buffers aren't cached so that
    // the ARM7 can use them without issues. We also need to ensure that the
    // ARM7 sees the updated values of the structure setup by LibXM7.
    DC_FlushAll();

    soundEnable();

    song_start(&module);

    while (1)
    {
        swiWaitForVBlank();

        {
            // Clear background
            uint16_t *bg_buf = bgGetGfxPtr(bg);
            memset(bg_buf, 0, 256 * 192 * 2);

            // Draw waveform on the main screen. We need to read the buffer from
            // an uncached mirror so that we don't load the buffer to the data
            // cache. We need the buffer to not be cached so that the ARM7 can
            // use it normally (the ARM7 can't see the ARM9 cache).
            u8 *wave_buf = memUncached(buffer_left);

            // Divide the buffer into 256 steps
            int step = (CAPTURE_BUFFER_SIZE * 4) / 256;
            for (int i = 0; i < 256; i ++)
            {
                // Convert from signed 8 bits to 0-91
                s32 val = wave_buf[i * step];
                int y = ((val ^ 0x80) * 192) >> 9;
                bg_buf[y * 256 + i] = RGB15(0, 31, 0) | BIT(15);
            }

            wave_buf = memUncached(buffer_right);

            for (int i = 0; i < 256; i ++)
            {
                // Convert from signed 8 bits to 0-91 and add 92
                s32 val = wave_buf[i * step];
                int y = (((val ^ 0x80) * 192) >> 9) + 92;
                bg_buf[y * 256 + i] = RGB15(0, 31, 0) | BIT(15);
            }

            capture_start(buffer_right, buffer_left);
        }

        scanKeys();

        uint16_t keys_down = keysDown();
        if (keys_down & KEY_START)
            break;
    }

    soundCaptureStop(0);
    soundCaptureStop(1);

    song_stop();
    swiWaitForVBlank();

    XM7_UnloadMOD(&module);

    soundDisable();

    return 0;
}
