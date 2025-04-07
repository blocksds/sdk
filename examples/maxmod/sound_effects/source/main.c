// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

#include <stdbool.h>
#include <stdio.h>

#include <maxmod9.h>
#include <nds.h>

#include "soundbank.h"
#include "soundbank_bin.h"

int main(int argc, char **argv)
{
    consoleDemoInit();

    mmInitDefaultMem((mm_addr)soundbank_bin);

    // Use extended mode (software + hardware mixing)
    mmSelectMode(MM_MODE_C);

    // Load a song that we will keep mute but wasting sound channels so that we
    // can test how to release sound channels. This song simply uses a lot of
    // channels with a constant sound so that channel usage is always the same.
    mmLoad(MOD_TEST_SONG);

    // Load sound effects
    mmLoadEffect(SFX_FIRE_EXPLOSION);
    mmLoadEffect(SFX_NATURE);

    // Make the test song almost mute
    mmSetModuleVolume(4);

    soundEnable();

    swiWaitForVBlank();

    printf("maxmod basic sounds example\n");
    printf("===========================\n");
    printf("\n");
    printf("A: Play explosion SFX\n");
    printf("\n");
    printf("B: Stop song\n");
    printf("\n");
    printf("START: Return to loader\n");

    mmStart(MOD_TEST_SONG, MM_PLAY_LOOP);

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint16_t keys_down = keysDown();

        if (keys_down & KEY_A)
        {
            // Play a sound with a random panning that can't be interrupted
            mm_sfxhand h = mmEffect(SFX_FIRE_EXPLOSION);
            mmEffectPanning(h, rand() & 255);
            mmEffectVolume(h, 96);
        }

        if (keys_down & KEY_B)
        {
            // Play a sound that can be interrupted if needed by other effects
            mm_sfxhand h = mmEffect(SFX_NATURE);
            mmEffectRelease(h);
        }

        if (keys_down & KEY_START)
            break;
    }

    soundDisable();

    return 0;
}
