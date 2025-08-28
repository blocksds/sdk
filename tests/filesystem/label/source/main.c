// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023
// SPDX-FileContributor: Adrian "asie" Siekierka, 2025

#include <errno.h>
#include <stdio.h>
#include <limits.h>

#include <nds.h>
#include <nds/arm9/dldi.h>
#include <fat.h>

void wait_press_button_a(void)
{
    while(1)
    {
        swiWaitForVBlank();

        scanKeys();

        if (keysDown() & KEY_A)
            break;
    }
}

void wait_forever(void)
{
    while(1)
        swiWaitForVBlank();
}

int main(int argc, char **argv)
{
    char root_path[PATH_MAX + 1];
    char label[FAT_VOLUME_LABEL_MAX + 1];

    consoleDemoInit();

    printf("DLDI name: %s\n", io_dldi_data->friendlyName);
    printf("DSi mode: %d\n", isDSiMode());

    bool init_ok = fatInitDefault();
    if (!init_ok)
    {
        perror("fatInitDefault()");
        wait_forever();
    }

    if (realpath("/", root_path) == NULL)
    {
        perror("realpath()");
        wait_forever();
    }

    printf("Drive: %s\n", root_path);

    if (!fatGetVolumeLabel(root_path, label))
    {
        printf("fatGetVolumeLabel() failed");
        wait_forever();
    }
    printf("\nfatGetVolumeLabel()\n\"%s\"\n", label);

    if (!fatSetVolumeLabel(root_path, "BLOCKSDS"))
    {
        printf("fatSetVolumeLabel() failed");
        wait_forever();
    }
    printf("\nfatSetVolumeLabel()\n");

    if (!fatGetVolumeLabel(root_path, label))
    {
        perror("fatGetVolumeLabel()");
        wait_forever();
    }
    printf("\nfatGetVolumeLabel()\n\"%s\"\n", label);

    printf("\nPress START to exit to loader\n");

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
