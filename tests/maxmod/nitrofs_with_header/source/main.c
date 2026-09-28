// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023-2026

#include <stdbool.h>
#include <stdio.h>

#include <filesystem.h>
#include <maxmod9.h>
#include <nds.h>

#include "soundbank.h"

// - lasse_haen_pyykit.xm
//
// XM module by Lasse. Obtained from the original libxm7 example by sverx
//
// - Parallax Glacier by Raina
//
// http://modarchive.org/index.php?request=view_by_moduleid&query=163194

__attribute__((noreturn)) void wait_forever(void)
{
    printf("\nPress START to return to loader\n");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();
        if (keysDown() & KEY_START)
            exit(1);
    }
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    printf("Maxmod NitroFS test\n");
    printf("\n");

    bool init_ok = nitroFSInit(NULL);
    if (!init_ok)
    {
        perror("nitroFSInit()");
        wait_forever();
    }

    printf("NitroFS init ok!\n");
    printf("\n");

    if (!mmInitDefault("nitro:/soundbank.bin"))
    {
        printf("mmInitDefault() failed\n");
        wait_forever();
    }

    printf("IDs to names:\n");
    printf("\n");
    printf("%d -> %s\n", MOD_PARALLAX_80599, mmGetModuleNameById(MOD_PARALLAX_80599));
    printf("%d -> %s\n", MOD_LASSE_HAEN_PYYKIT, mmGetModuleNameById(MOD_LASSE_HAEN_PYYKIT));
    printf("%d -> %s\n", SFX_NATURE, mmGetSampleNameById(SFX_NATURE));
    printf("%d -> %s\n", SFX_FIRE_EXPLOSION, mmGetSampleNameById(SFX_FIRE_EXPLOSION));
    printf("\n");
    printf("Names to IDs:\n");
    printf("\n");
    printf("%s -> %ld\n", "parallax_80599.xm", mmGetModuleIdByName("parallax_80599.xm"));
    printf("%s -> %ld\n", "lasse_haen_pyykit.xm", mmGetModuleIdByName("lasse_haen_pyykit.xm"));
    printf("%s -> %ld\n", "nature.wav", mmGetSampleIdByName("nature.wav"));
    printf("%s -> %ld\n", "fire_explosion.wav", mmGetSampleIdByName("fire_explosion.wav"));

    printf("\n");
    printf("\n");
    printf("START: Return to loader\n");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint16_t keys_down = keysDown();
        if (keys_down & KEY_START)
            break;
    }

    return 0;
}
