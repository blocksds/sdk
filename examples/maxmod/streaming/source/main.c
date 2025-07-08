// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Michele Di Giorgio, 2024
// SPDX-FileContributor: Antonio Niño Díaz, 2025
//
// Example usage of maxmod streaming. The example opens a WAV file and streams
// the data through maxmod's utilities. The main loop of the application reads
// the WAV file and saves the data read into a circular buffer. The streaming
// callback of Maxmod reads from the circular buffer and passes it to Maxmod.
// This is done because the Maxmod callback runs inside an interrupt handler,
// and interrupt handlers shouldn't do file reads.

#include <stdio.h>

#include <filesystem.h>
#include <maxmod9.h>
#include <nds.h>
#include <nds/arm9/dldi.h>

#define DATA_ID 0x61746164
#define FMT_ID  0x20746d66
#define RIFF_ID 0x46464952
#define WAVE_ID 0x45564157

typedef struct WAVHeader
{
    // "RIFF" chunk descriptor
    uint32_t chunkID;
    uint32_t chunkSize;
    uint32_t format;
    // "fmt" subchunk
    uint32_t subchunk1ID;
    uint32_t subchunk1Size;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    // "data" subchunk
    uint32_t subchunk2ID;
    uint32_t subchunk2Size;
}
WAVHeader_t;

#define BUFFER_LENGTH 16384

FILE *wavFile = NULL;

char stream_buffer[BUFFER_LENGTH];
int stream_buffer_in;
int stream_buffer_out;

mm_word streamingCallback(mm_word length,
                          mm_addr dest,
                          mm_stream_formats format)
{
    size_t multiplier = 0;

    if (format == MM_STREAM_8BIT_MONO)
        multiplier = 1;
    else if (format == MM_STREAM_8BIT_STEREO)
        multiplier = 2;
    else if (format == MM_STREAM_16BIT_MONO)
        multiplier = 2;
    else if (format == MM_STREAM_16BIT_STEREO)
        multiplier = 4;

    size_t size = length * multiplier;

    size_t bytes_until_end = BUFFER_LENGTH - stream_buffer_out;

    if (bytes_until_end > size)
    {
        char *src_ = &stream_buffer[stream_buffer_out];

        memcpy(dest, src_, size);
        stream_buffer_out += size;
    }
    else
    {
        char *src_ = &stream_buffer[stream_buffer_out];
        char *dst_ = dest;

        memcpy(dst_, src_, bytes_until_end);
        dst_ += bytes_until_end;
        size -= bytes_until_end;

        src_ = &stream_buffer[0];
        memcpy(dst_, src_, size);
        stream_buffer_out = size;
    }

    return length;
}

// This reads bytes from wavFile into the provided buffer. If the end of the
// file is reached, it starts from the start again.
void readFile(char *buffer, size_t size)
{
    while (size > 0)
    {
        int res = fread(buffer, 1, size, wavFile);
        size -= res;
        buffer += res;

        if (feof(wavFile))
        {
            // Loop back when song ends
            fseek(wavFile, sizeof(WAVHeader_t), SEEK_SET);
            res = fread(buffer, 1, size, wavFile);
            size -= res;
            buffer += res;

            printf("Restarting...\n");
        }
    }
}

void streamingFillBuffer(bool force_fill)
{
    if (!force_fill)
    {
        if (stream_buffer_in == stream_buffer_out)
            return;
    }

    if (stream_buffer_in < stream_buffer_out)
    {
        size_t size = stream_buffer_out - stream_buffer_in;
        readFile(&stream_buffer[stream_buffer_in], size);
        stream_buffer_in += size;
    }
    else
    {
        size_t size = BUFFER_LENGTH - stream_buffer_in;
        readFile(&stream_buffer[stream_buffer_in], size);
        stream_buffer_in = 0;

        size = stream_buffer_out - stream_buffer_in;
        readFile(&stream_buffer[stream_buffer_in], size);
        stream_buffer_in += size;
    }

    if (stream_buffer_in >= BUFFER_LENGTH)
        stream_buffer_in -= BUFFER_LENGTH;
}

int checkWAVHeader(const WAVHeader_t header)
{
    if (header.chunkID != RIFF_ID)
    {
        printf("Wrong RIFF_ID %lx\n", header.chunkID);
        return 1;
    }

    if (header.format != WAVE_ID)
    {
        printf("Wrong WAVE_ID %lx\n", header.format);
        return 1;
    }

    if (header.subchunk1ID != FMT_ID)
    {
        printf("Wrong FMT_ID %lx\n", header.subchunk1ID);
        return 1;
    }

    if (header.subchunk2ID != DATA_ID)
    {
        printf("Wrong Subchunk2ID %lx\n", header.subchunk2ID);
        return 1;
    }

    return 0;
}

mm_stream_formats getMMStreamType(uint16_t numChannels, uint16_t bitsPerSample)
{
    if (numChannels == 1)
    {
        if (bitsPerSample == 8)
            return MM_STREAM_8BIT_MONO;
        else
            return MM_STREAM_16BIT_MONO;
    }
    else if (numChannels == 2)
    {
        if (bitsPerSample == 8)
            return MM_STREAM_8BIT_STEREO;
        else
            return MM_STREAM_16BIT_STEREO;
    }
    return MM_STREAM_8BIT_MONO;
}

void waitForever(void)
{
    while (1)
        swiWaitForVBlank();
}

int main(int argc, char **argv)
{
    defaultExceptionHandler();

    consoleDemoInit();

    printf("maxmod streaming example\n");
    printf("===========================\n");
    printf("\n");

    if (!isDSiMode())
    {
        printf("Running on NDS.\n");
        printf("\n");
        printf("Choose filesystem access mode:\n");

        printf("A: Autodetect\n");
        printf("L: Force ARM7\n");
        printf("R: Force ARM9\n");

        while (1)
        {
            swiWaitForVBlank();

            scanKeys();

            uint32_t keys_down = keysDown();
            if (keys_down & KEY_A)
            {
                dldiSetMode(DLDI_MODE_AUTODETECT);
                break;
            }

            if (keys_down & KEY_L)
            {
                dldiSetMode(DLDI_MODE_ARM7);
                break;
            }

            if (keys_down & KEY_R)
            {
                dldiSetMode(DLDI_MODE_ARM9);
                break;
            }
        }

        printf("\n");
        printf("DLDI_MODE: %d\n", dldiGetMode());
        printf("\n");
    }

    printf("START: Return to loader\n");

    bool initOK = nitroFSInit(NULL);
    if (!initOK)
    {
        perror("nitroFSInit");
        return 1;
    }

    wavFile = fopen("nitro:/AuldLangSyne.wav", "rb");
    if (wavFile == NULL)
    {
        perror("fopen");
        waitForever();
    }

    WAVHeader_t wavHeader = { 0 };
    if (fread(&wavHeader, 1, sizeof(WAVHeader_t), wavFile) != sizeof(WAVHeader_t))
    {
        perror("fread");
        waitForever();
    }
    if (checkWAVHeader(wavHeader) != 0)
    {
        printf("WAV file header is corrupt!\n");
        waitForever();
    }

    // Fill the buffer before we start doing anything
    streamingFillBuffer(true);

    // We are not using a soundbank so we need to manually initialize
    // mm_ds_system.
    mm_ds_system mmSys =
    {
        .mod_count    = 0,
        .samp_count   = 0,
        .mem_bank     = 0,
        .fifo_channel = FIFO_MAXMOD
    };
    mmInit(&mmSys);

    // Open the stream
    mm_stream stream =
    {
        .sampling_rate = wavHeader.sampleRate,
        .buffer_length = 2048,
        .callback      = streamingCallback,
        .format        = getMMStreamType(wavHeader.numChannels, wavHeader.bitsPerSample),
        .timer         = MM_TIMER0,
        .manual        = false,
    };
    mmStreamOpen(&stream);

    while (1)
    {
        swiWaitForVBlank();

        // Read file until we have filled the circular buffer
        streamingFillBuffer(false);

        scanKeys();

        uint16_t keys_down = keysDown();

        if (keys_down & KEY_START)
            break;
    }

    mmStreamClose();

    if (fclose(wavFile) != 0)
    {
        perror("fclose");
        waitForever();
    }

    soundDisable();

    return 0;
}
