// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

// Random file generated with:
//
//     dd bs=1024 count=2048 < /dev/urandom > random.bin

//#include <errno.h>
#include <stdio.h>

#include <fatfs.h>
#include <nds.h>

#include <nds/arm9/dldi.h>

#include "md5/md5.h"

uint32_t my_rand(void)
{
    static uint32_t seed = 0;

    seed = seed * 0xFDB97531 + 0x2468ACE;

    return seed;
}

#define MAX_SECTORS 5
static char input_buffer[512 * MAX_SECTORS];

int main(int argc, char **argv)
{
    consoleDemoInit();

    printf("\x1b[2J"); // Clear console

    printf("DLDI name:\n");
    printf("%s\n", io_dldi_data->friendlyName);
    printf("\n");
    printf("DSi mode: %d\n", isDSiMode());

    fflush(stdout);

    bool init_ok = nitroFSInit(NULL);
    if (!init_ok)
    {
        perror("nitroFSInit()");
        goto exit;
    }


    chdir("nitro:/");
    printf("Current dir: %s\n\n", getcwd(NULL, 0));

    uint64_t ticks = 0;

    cpuStartTiming(0);
    FILE *f = fopen("random.bin", "r");
    ticks += cpuEndTiming();
    if (f == NULL)
    {
        perror("fopen(random.bin)");
        goto exit;
    }

    printf("Calculating MD5: ");

    size_t input_size = 0;

    MD5Context ctx;
    md5Init(&ctx);

    uint32_t size = 0;

    while (1)
    {
        cpuStartTiming(0);
        input_size = fread(input_buffer, 1, 1024, f);
        ticks += cpuEndTiming();

        if (input_size <= 0)
            break;

        md5Update(&ctx, (uint8_t *)input_buffer, input_size);

        size += input_size;
        if ((size % (1024 * 1024)) == 0)
            printf(".");
    }
    printf("\n");
    printf("\n");

    md5Finalize(&ctx);

    uint8_t digest[16];
    memcpy(digest, ctx.digest, 16);

    for (int i = 0; i < 16; i++)
        printf("%02X", digest[i]);
    printf("\n");

    // Now, try random accesses

    uint32_t num_sectors = size / 512;

    printf("Random reads: ");
    for (int i = 0; i < 10000; i++)
    {
        uint32_t sector = rand() % (num_sectors - MAX_SECTORS);
        uint32_t count = rand() % MAX_SECTORS;

        uint32_t offset = sector * 512;
        uint32_t size = count * 512;

        cpuStartTiming(0);
        if (fseek(f, offset, SEEK_SET) != 0)
            printf("S");
        if (fread(input_buffer, 1, size, f) != size)
            printf("X");
        ticks += cpuEndTiming();

        if ((i % 100) == 0)
            printf(".");
    }
    printf("\n");

    fclose(f);

    printf("\n");
    printf("Time: %llu ticks\n", ticks);
    printf("      %llu seconds\n", ticks / BUS_CLOCK);

exit:
    printf("\n");
    printf("Press START to exit to loader\n");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint32_t keys_down = keysDown();
        if (keys_down & KEY_START)
            break;
    }

    return 0;
}
