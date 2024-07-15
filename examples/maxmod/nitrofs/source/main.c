// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

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
        while (1)
        {
            swiWaitForVBlank();
            uint16_t keys = keysDown();
            if (keys & KEY_START)
                return 0;
        }
    }

    printf("NitroFS init ok!\n");
    printf("\n");
    printf("\n");

    printf("X: haen pyykit by Lasse\n");
    printf("Y: Parallax Glacier by Raina\n");
    printf("\n");
    printf("B: Stop song\n");
    printf("\n");

    mmInitDefault("nitro:/soundbank.bin");

    mmLoad(MOD_PARALLAX_80599);
    mmLoad(MOD_LASSE_HAEN_PYYKIT);

    soundEnable();

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

    soundDisable();

    return 0;
}
