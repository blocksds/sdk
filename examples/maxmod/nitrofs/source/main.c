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

    printf("maxmod NitroFS example\n");
    printf("======================\n");
    printf("\n");
    printf("START: Return to loader\n");
    printf("\n");
    printf("\n");

    // It isn't needed to call fatInitDefault() manually. If nitroFSInit detects
    // that the ROM is running in a flashcard or from the DSi internal SD slot,
    // it will call it internally.
    bool init_ok = nitroFSInit(NULL);
    if (!init_ok)
    {
        perror("nitroFSInit()");
        wait_forever();
    }

    printf("NitroFS init ok!\n");
    printf("\n");
    printf("\n");

    soundEnable();

    printf("X: haen pyykit by Lasse\n");
    printf("Y: Parallax Glacier by Raina\n");
    printf("\n");
    printf("B: Stop song\n");
    printf("\n");

    if (!mmInitDefault("nitro:/soundbank.bin"))
    {
        printf("mmInitDefault() failed\n");
        wait_forever();
    }

    int ret = mmLoad(MOD_PARALLAX_80599);
    if (ret != 0)
    {
        printf("mmLoad(1): %d\n", ret);
        wait_forever();
    }

    ret = mmLoad(MOD_LASSE_HAEN_PYYKIT);
    if (ret != 0)
    {
        printf("mmLoad(2): %d\n", ret);
        wait_forever();
    }

    bool playing = false;

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint16_t keys_down = keysDown();

        if (keys_down & KEY_B)
        {
            if (playing)
            {
                mmStop();
                playing = false;
            }
        }

        if (keys_down & KEY_X)
        {
            if (playing)
                mmStop();

            mmStart(MOD_LASSE_HAEN_PYYKIT, MM_PLAY_LOOP);
            playing = true;
        }

        if (keys_down & KEY_Y)
        {
            if (playing)
                mmStop();

            mmStart(MOD_PARALLAX_80599, MM_PLAY_LOOP);
            playing = true;
        }

        if (keys_down & KEY_START)
            break;
    }

    ret = mmUnload(MOD_PARALLAX_80599);
    if (ret != 0)
    {
        printf("mmUnload(1): %d\n", ret);
        wait_forever();
    }

    ret = mmUnload(MOD_LASSE_HAEN_PYYKIT);
    if (ret != 0)
    {
        printf("mmUnload(2): %d\n", ret);
        wait_forever();
    }

    soundDisable();

    return 0;
}
