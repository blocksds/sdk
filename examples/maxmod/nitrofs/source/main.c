// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023-2026

// This example shows how to use a soundbank stored in NitroFS. Samples and
// modules have an ID that you need to use to load them and play them. Normally,
// the Makefiles of BlocksDS generate a header with definitions. However, when
// the soundbank is saved to NitroFS, the makefiles also add a dictionary with
// names and their IDs to the soundbank, so you don't need to use the header.
//
// Using the definitions from the header is faster, but it can also be less
// flexible.

#include <stdbool.h>
#include <stdio.h>

#include <filesystem.h>
#include <maxmod9.h>
#include <nds.h>

// This is only needed for MOD_xxx and SFX_xxx definitions. If you don't use
// them, you can remove this include.
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

    if (!mmInitDefault("nitro:/soundbank.bin"))
    {
        printf("mmInitDefault() failed\n");
        wait_forever();
    }

    // Load a module using the ID from the definitions header
    int ret = mmLoad(MOD_PARALLAX_80599);
    if (ret != 0)
    {
        printf("mmLoad(1): %d\n", ret);
        wait_forever();
    }

    // Load a module using the ID from soundbank dictionary
    mm_sword lasse_haen_pyykit_id = mmGetModuleIdByName("lasse_haen_pyykit.xm");
    if (lasse_haen_pyykit_id == -1)
    {
        printf("mmGetModuleIdByName(): %ld\n", lasse_haen_pyykit_id);
        wait_forever();
    }

    ret = mmLoad(lasse_haen_pyykit_id);
    if (ret != 0)
    {
        printf("mmLoad(2): %d\n", ret);
        wait_forever();
    }

    // Load a sound effect using the ID from the definitions header
    ret = mmLoadEffect(SFX_FIRE_EXPLOSION);
    if (ret != 0)
    {
        printf("mmLoadEffect(1): %d\n", ret);
        wait_forever();
    }

    // Load a sound effect using the ID from soundbank dictionary
    mm_sword nature = mmGetSampleIdByName("nature.wav");
    if (nature == -1)
    {
        printf("mmGetSampleIdByName(): %ld\n", nature);
        wait_forever();
    }

    ret = mmLoadEffect(nature);
    if (ret != 0)
    {
        printf("mmLoadEffect(2): %d\n", ret);
        wait_forever();
    }

    printf("X: haen pyykit by Lasse\n");
    printf("Y: Parallax Glacier by Raina\n");
    printf("\n");
    printf("B: Stop song\n");
    printf("\n");
    printf("L: Play explosion\n");
    printf("R: Play nature\n");
    printf("\n");
    printf("A: Stop sound effects\n");

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

        if (keys_down & KEY_A)
            mmEffectCancelAll();

        if (keys_down & KEY_L)
            mmEffect(SFX_FIRE_EXPLOSION);

        if (keys_down & KEY_R)
            mmEffect(nature);

        if (keys_down & KEY_START)
            break;
    }

    ret = mmUnload(MOD_PARALLAX_80599);
    if (ret != 0)
    {
        printf("mmUnload(1): %d\n", ret);
        wait_forever();
    }

    ret = mmUnload(lasse_haen_pyykit_id);
    if (ret != 0)
    {
        printf("mmUnload(2): %d\n", ret);
        wait_forever();
    }

    ret = mmUnloadEffect(SFX_FIRE_EXPLOSION);
    if (ret != 0)
    {
        printf("mmUnloadEffect(1): %d\n", ret);
        wait_forever();
    }

    ret = mmUnloadEffect(nature);
    if (ret != 0)
    {
        printf("mmUnloadEffect(2): %d\n", ret);
        wait_forever();
    }

    soundDisable();

    return 0;
}
