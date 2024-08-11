// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Michele Di Giorgio, 2024
//
// Example usage of maxmod streaming. The example loads a WAV file and streams
// the data through maxmod's utilities.
//
// Audio track credits:
//  https://www.looperman.com/loops/detail/222561/kingmike16-120bpm-techno-synth-loop

#include <stdio.h>

#include <filesystem.h>
#include <maxmod9.h>
#include <nds.h>

#define WAV_FILENAME "nitro:/looperman.wav"

#define DATA_ID 0x61746164
#define FMT_ID  0x20746d66
#define RIFF_ID 0x46464952
#define WAVE_ID 0x45564157

FILE *wavFile = NULL;

typedef struct WAVHeader {
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
} WAVHeader_t;

mm_word streamingCallback(mm_word length,
                          mm_addr dest,
                          mm_stream_formats format)
{
    // 16-bit, 2 channels (stereo), so we need to output 4 bytes for each word
    int res = fread(dest, 4, length, wavFile);

    if (feof(wavFile)) {
        // Loop back when song ends
        fseek(wavFile, sizeof(WAVHeader_t), SEEK_SET);
        res = fread(dest, 4, length, wavFile);
    }
    // Return the number of words streamed so that if this number is less than
    // "length", the rest will be buffered and used in the next stream
    return res;
}

int checkWAVHeader(const WAVHeader_t header)
{
    if (header.chunkID != RIFF_ID) {
        printf("Wrong RIFF_ID %lx\n", header.chunkID);
        return 1;
    }

    if (header.format != WAVE_ID) {
        printf("Wrong WAVE_ID %lx\n", header.format);
        return 1;
    }

    if (header.subchunk1ID != FMT_ID) {
        printf("Wrong FMT_ID %lx\n", header.subchunk1ID);
        return 1;
    }

    if (header.subchunk2ID != DATA_ID) {
        printf("Wrong Subchunk2ID %lx\n", header.subchunk2ID);
        return 1;
    }

    return 0;
}

mm_stream_formats getMMStreamType(uint16_t numChannels, uint16_t bitsPerSample)
{
    if (numChannels == 1) {
        if (bitsPerSample == 8) {
            return MM_STREAM_8BIT_MONO;
        } else {
            return MM_STREAM_16BIT_MONO;
        }
    } else if (numChannels == 2) {
        if (bitsPerSample == 8) {
            return MM_STREAM_8BIT_STEREO;
        } else {
            return MM_STREAM_16BIT_STEREO;
        }
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
    consoleDemoInit();

    printf("maxmod streaming example\n");
    printf("===========================\n");
    printf("\n");
    printf("START: Return to loader\n");

    bool initOK = nitroFSInit(NULL);
    if (!initOK) {
        printf("nitroFSInit() error!\n");
        return 1;
    }

    wavFile = fopen(WAV_FILENAME, "rb");
    if (wavFile == NULL) {
        printf("fopen(%s) failed!\n", WAV_FILENAME);
        waitForever();
    }

    WAVHeader_t wavHeader = { 0 };
    if (fread(&wavHeader, 1, sizeof(WAVHeader_t), wavFile) == 0) {
        perror("fread");
        waitForever();
    }
    if (checkWAVHeader(wavHeader) != 0) {
        printf("WAV file header is corrupt!\n");
        waitForever();
    }

    // We are not using soundbank so we need to manually initialize
    // mm_ds_system.
    mm_ds_system mmSys;
    mmSys.mod_count    = 0;
    mmSys.samp_count   = 0;
    mmSys.mem_bank     = 0;
    mmSys.fifo_channel = FIFO_MAXMOD;
    mmInit(&mmSys);

    // Open the stream
    mm_stream stream;
    stream.sampling_rate = wavHeader.sampleRate;
    stream.buffer_length = 16000;
    stream.callback      = streamingCallback;
    stream.format        = getMMStreamType(wavHeader.numChannels,
                                           wavHeader.bitsPerSample);
    stream.timer         = MM_TIMER0;
    stream.manual        = false;
    mmStreamOpen(&stream);

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint16_t keys_down = keysDown();

        if (keys_down & KEY_START)
            break;
    }

    mmStreamClose();

    if (fclose(wavFile) != 0) {
        perror("fclose");
        waitForever();
    }

    soundDisable();

    return 0;
}
