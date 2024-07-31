// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023
// SPDX-FileContributor: Michele Di Giorgio, 2024

#include <stdbool.h>
#include <stdio.h>

#include <maxmod9.h>
#include <nds.h>

#include "soundbank.h"
#include "soundbank_bin.h"

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

    mmInitDefaultMem((mm_addr)soundbank_bin);

    mmLoad(MOD_PARALLAX_80599);
    mmLoad(MOD_LASSE_HAEN_PYYKIT);

    printf("maxmod basic sounds example\n");
    printf("===========================\n");
    printf("\n");
    printf("X: haen pyykit by Lasse\n");
    printf("Y: Parallax Glacier by Raina\n");
    printf("A: Play explosion SFX\n");
    printf("\n");
    printf("B: Stop song\n");
    printf("\n");
    printf("START: Return to loader\n");

    // Load sound effects
    mmLoadEffect(SFX_FIRE_EXPLOSION);

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

        if (keys_down & KEY_A)
            mmEffect(SFX_FIRE_EXPLOSION);

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
