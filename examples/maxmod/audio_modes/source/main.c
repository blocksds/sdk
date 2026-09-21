// SPDX-License-Identiier: CC0-1.0
//
// SPDX-FileContributor: Michele Di Giorgio, 2024
// SPDX-FileContributor: Antonio Niño Díaz, 2026

// Example usage of the following audio modes:
// - MM_MODE_A, Complete hardware mixing mode
// - MM_MODE_B, Interpolated mixing mode
// - MM_MODE_C, Extended mixing mode
//
// Check https://blocksds.skylyrac.net/docs/maxmod/group__nds__arm9__init.html
// for more information about the available modes.
//
// - Parallax Glacier by Raina
//
// http://modarchive.org/index.php?request=view_by_moduleid&query=163194

#include <maxmod9.h>
#include <nds.h>

#include "soundbank.h"
#include "soundbank_bin.h"

#include <stdio.h>

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

    printf("maxmod audio modes example\n");
    printf("==========================\n");
    printf("\n");

    soundEnable();

    if (!mmInitDefaultMem((mm_addr)soundbank_bin))
    {
        printf("mmInitDefaultMem() failed\n");
        wait_forever();
    }

    int ret = mmLoad(MOD_PARALLAX_80599);
    if (ret != 0)
    {
        printf("mmLoad(): %d\n", ret);
        wait_forever();
    }

    printf("A: Hardware mixing (mode A)\n");
    printf("B: Interpolated mixing (mode B)\n");
    printf("Y: Extended mixing (mode C)\n");
    printf("\n");
    printf("L: Start\n");
    printf("R: Stop\n");
    printf("\n");
    printf("START: Return to loader\n");

    mmStart(MOD_PARALLAX_80599, MM_PLAY_LOOP);

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint16_t keys_down = keysDown();

        if (keys_down & KEY_START)
            break;

        if (keys_down & KEY_A)
            mmSelectMode(MM_MODE_A);
        if (keys_down & KEY_B)
            mmSelectMode(MM_MODE_B);
        if (keys_down & KEY_Y)
            mmSelectMode(MM_MODE_C);

        if (keys_down & KEY_L)
            mmStart(MOD_PARALLAX_80599, MM_PLAY_LOOP);
        if (keys_down & KEY_R)
            mmStop();
    }

    mmStop();

    ret = mmUnload(MOD_PARALLAX_80599);
    if (ret != 0)
    {
        printf("mmUnload(): %d\n", ret);
        wait_forever();
    }

    soundDisable();

    return 0;
}
