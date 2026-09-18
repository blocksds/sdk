// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023-2026
// SPDX-FileContributor: Adrian "asie" Siekierka, 2026

#include <stdio.h>

#include <nds.h>
#include <nds/arm9/dldi.h>
#include <fat.h>

int main(int argc, char **argv)
{
    char buf[FAT_SHORT_FILE_NAME_MAX+1];

    consoleDemoInit();

    printf("DLDI name: %s\n", io_dldi_data->friendlyName);
    printf("DSi mode: %d\n", isDSiMode());

    bool init_ok = fatInitDefault();
    if (!init_ok)
    {
        perror("fatInitDefault()");
        goto exit;
    }

    if ((argc < 1) || (argv == NULL) || (argv[0] == NULL))
    {
        printf("ERROR: missing argv");
        goto exit;
    }

    if (!FAT_getShortNameFor(argv[0], buf))
    {
        perror("FAT_getShortNameFor()");
        goto exit;
    }

    printf("\n\nFAT_getShortNameFor(argv[0])\n\n%s\n\n", buf);

exit:
    printf("\n\nPress START to exit to loader\n");

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
