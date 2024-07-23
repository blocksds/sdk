// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <stdio.h>

#include <nds.h>

// The sample rate used for the recording (samples per second)
#define SAMPLE_RATE  8000

// Size of the buffer used to store the full recording. If you want to record N
// seconds of audio, you need:
//
//     Samples per second * size of a sample * number of seconds
//
// The size of a sample can be either 8 bits or 16 bits depending on the
// arguments passed to soundMicRecord().
#define SOUND_BUFFER_SIZE (SAMPLE_RATE * 2 * 3)

// The recording buffer is used to store the full recording.
uint8_t recording_buffer[SOUND_BUFFER_SIZE];

// This keeps the next index in the recording_buffer where the next samples will
// be stored.
unsigned int recording_pointer;

// This buffer is used to store a copy of the recording to play it back.
uint8_t playback_buffer[SOUND_BUFFER_SIZE];

// This is the size of the temporary buffer that the ARM7 will use to record
// audio. When the callback is called, you will get a pointer to some address
// inside this buffer. It's your responsibility to copy the data out of this
// buffer to another buffer if you want to use that data.
//
// Note that this is a double buffer, so the size needs to be double the size of
// the time you want to record each callback. For example, if you want the
// callback to be called every 2 frames:
//
//     samples rate * number of channels * number of seconds
//
// The number of seconds is 1 / 30 or (2 / 60).
#define MICROPHONE_BUFFER_SIZE (SAMPLE_RATE * 2 / 30)

uint16_t temporary_buffer[MICROPHONE_BUFFER_SIZE];

// This flag controls whether the callback will copy data to the recording
// buffer or if it will just ignore them (they will still be displayed on the
// top screen!).
bool recording = false;

void microphone_handler(void *completed_buffer, int length)
{
    if (!recording)
        return;

    if (recording_pointer >= SOUND_BUFFER_SIZE)
    {
        recording = false;
        return;
    }

    if (recording_pointer + length > SOUND_BUFFER_SIZE)
        length = SOUND_BUFFER_SIZE - recording_pointer;

    dmaCopy(completed_buffer, recording_buffer + recording_pointer, length);

    recording_pointer += length;
}

int main(int argc, char *argv[])
{
    // Setup the top screen to display the waveform as a bitmap
    videoSetMode(MODE_5_2D);
    vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
    int bg = bgInit(2, BgType_Bmp16, BgSize_B16_256x256, 0, 0);

    consoleDemoInit();

    soundEnable();

    // We need to ensure that the temporary buffer isn't cached so that the ARM7
    // can use it without issues. We also need to flush the recording and
    // playback buffers so that we can use DMA between them. We also need to
    // flush the playback buffer so that the audio hardware can see the
    // up-to-date values of the buffer always.
    DC_FlushAll();

    // The microphone (especially on the DS) requires about a second to get its
    // input levels to a valid baseline. Normally you could just discard the
    // first half second, but in this example we enable it at the beginning of
    // main() so that we can always draw the waveform.
    soundMicRecord(temporary_buffer, sizeof(temporary_buffer),
                   MicFormat_12Bit, SAMPLE_RATE, microphone_handler);

    while (1)
    {
        swiWaitForVBlank();

        // Draw waveform on the main screen. We need to read the buffer from an
        // uncached mirror so that we don't load the buffer to the data cache.
        // We need the buffer to not be cached so that the ARM7 can use it
        // normally (the ARM7 can't see the ARM9 cache).
        s16 *wave_buf = memUncached(temporary_buffer);

        // Clear background
        uint16_t *bg_buf = bgGetGfxPtr(bg);
        memset(bg_buf, 0, 256 * 192 * 2);

        // Divide the buffer into 256 steps (but there are two channels, so
        // consider that too).
        int step = (MICROPHONE_BUFFER_SIZE / 2) / 256;
        for (int i = 0; i < 256; i ++)
        {
            // Convert from signed 16 bits to 0-191
            s32 val = wave_buf[i * step];
            int y = ((0x8000 + val) * 192) >> 16;
            bg_buf[y * 256 + i] = RGB15(0, 31, 0) | BIT(15);
        }

        consoleClear();

        printf("A: Record\n");
        printf("B: Play\n");
        printf("\n");

        if (recording)
            printf("Recording... %u/%u\n", recording_pointer, SOUND_BUFFER_SIZE);
        else
            printf("Ready!\n");

        scanKeys();
        uint16_t keys_down = keysDown();

        if (keys_down & KEY_A)
        {
            recording_pointer = 0;
            recording = true;
        }

        if (keys_down & KEY_B)
        {
            recording = false;

            // Copy the last recorded waveform to a buffer to send to the ARM7
            // to play it back.
            dmaCopy(recording_buffer, playback_buffer, SOUND_BUFFER_SIZE);

            soundPlaySample(playback_buffer, SoundFormat_16Bit, SOUND_BUFFER_SIZE,
                            SAMPLE_RATE, 127, 64, false, 0);
        }

        if (keys_down & KEY_START)
            break;
    }

    // Turn off the microphone when you're done.
    soundMicOff();
}
