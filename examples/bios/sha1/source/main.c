// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

// Random file generated with:
//
//     dd bs=1024 count=16 < /dev/urandom > random.bin
//
// Then get the SHA1 sum with:
//
//     sha1sum random.bin
//
// Then, update the array ``expected[20]``.

#include <stdio.h>

#include <nds.h>

#include "random_bin.h"

int main(int argc, char **argv)
{
    consoleDemoInit();

    if (!isDSiMode())
    {
        printf("SHA1 BIOS functions are only\n");
        printf("available on DSi\n");
    }
    else
    {
        const uint8_t expected[20] = {
            0x14, 0xbf, 0xe7, 0x82, 0x4f, 0x70, 0x77, 0xe8, 0x66, 0x0b,
            0xdd, 0x57, 0x24, 0x52, 0x73, 0x0e, 0xe5, 0x4c, 0xc9, 0x4f
        };

        printf("Expected SHA1:\n\n");
        for (int i = 0; i < 20; i++)
            printf("%02X", expected[i]);
        printf("\n\n");

        uint8_t digest[20] = { 0 };

        swiSHA1Calc(digest, random_bin, random_bin_size);

        printf("Calculated SHA1:\n\n");
        for (int i = 0; i < 20; i++)
            printf("%02X", digest[i]);
        printf("\n\n");

        int match = swiSHA1Verify(digest, expected);

        printf("Do they match? %s\n", match ? "Yes" : "No");
    }
    printf("\n");
    printf("Press START to exit");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();
        if (keysDown() & KEY_START)
            break;
    }

    return 0;
}
