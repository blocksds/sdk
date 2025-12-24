// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <nds.h>

#include "city_uncompressed_bin.h"
#include "city_huffman_bin.h"
#include "city_lzss_bin.h"
#include "city_rle_bin.h"

// This buffer is used to decompress our compressed files and to verify that
// they have been decompressed correctly. It's too big for the stack, though.
static uint8_t buffer[128 * 128 * 2];

// Direct decompression functions
// ==============================

void direct_decompression_tests(void)
{
    printf("DIRECT\n");
    printf("======\n");
    printf("\n");

    memset(buffer, 0, sizeof(buffer));
    decompress(city_huffman_bin, &buffer[0], HUFF);
    if (memcmp(city_uncompressed_bin, buffer, sizeof(buffer)) == 0)
        printf("HUFFMAN: OK\n");
    else
        printf("HUFFMAN: BAD\n");

    memset(buffer, 0, sizeof(buffer));
    decompress(city_lzss_bin, &buffer[0], LZ77);
    if (memcmp(city_uncompressed_bin, buffer, sizeof(buffer)) == 0)
        printf("LZSS: OK\n");
    else
        printf("LZSS: BAD\n");

    memset(buffer, 0, sizeof(buffer));
    decompress(city_rle_bin, &buffer[0], RLE);
    if (memcmp(city_uncompressed_bin, buffer, sizeof(buffer)) == 0)
        printf("RLE: OK\n");
    else
        printf("RLE: BAD\n");

    printf("\n");
}

// Decompression stream functions
// ==============================

// Called to get the header of the stream.
static int get_header(uint8_t *source, uint16_t *dest, uint32_t arg)
{
    (void)dest;
    (void)arg;

    return *(uint32_t *)source;
}

// It gets called to get a byte of the compressed data.
static uint8_t read_8(uint8_t *source)
{
    return *source;
}

// Called to get a halfword of the compressed data. Unused.
static uint16_t read_16(uint16_t *source)
{
    return *source;
}

// Called to get a word of the compressed data. Used for Huffman.
static uint32_t read_32(uint32_t *source)
{
    return *source;
}

TDecompressionStream stream = {
    get_header,
    NULL, // Called to verify the result afterwards. It can be NULL.
    read_8,
    read_16,
    read_32
};

void decompression_stream_tests(void)
{
    printf("STREAMS\n");
    printf("=======\n");
    printf("\n");

    // Only LZ77Vram, HUFF and RLEVram support streaming.

    // For Huffman, make sure to pass a 512 byte buffer in 'param' to be used as
    // a temporary buffer by the decompression code.
    memset(buffer, 0, sizeof(buffer));
    uint32_t huffman_temp_buffer[512 / sizeof(uint32_t)];
    decompressStreamStruct(city_huffman_bin, &buffer[0], HUFF,
                           &huffman_temp_buffer[0], &stream);
    if (memcmp(city_uncompressed_bin, buffer, sizeof(buffer)) == 0)
        printf("HUFFMAN: OK\n");
    else
        printf("HUFFMAN: BAD\n");

    memset(buffer, 0, sizeof(buffer));
    decompressStreamStruct(city_lzss_bin, &buffer[0], LZ77Vram,
                           NULL, &stream);
    if (memcmp(city_uncompressed_bin, buffer, sizeof(buffer)) == 0)
        printf("LZSS: OK\n");
    else
        printf("LZSS: BAD\n");

    memset(buffer, 0, sizeof(buffer));
    decompressStreamStruct(city_rle_bin, &buffer[0], RLEVram,
                           NULL, &stream);
    if (memcmp(city_uncompressed_bin, buffer, sizeof(buffer)) == 0)
        printf("RLE: OK\n");
    else
        printf("RLE: BAD\n");

    printf("\n");
}

// Main test routine
// =================

int main(int argc, char *argv[])
{
    consoleDemoInit();

    direct_decompression_tests();
    decompression_stream_tests();

    printf("START: Exit to loader\n");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();
        if (keysHeld() & KEY_START)
            break;
    }

    return 0;
}
