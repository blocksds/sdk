// SPDX-License-Identiier: CC0-1.0
//
// SPDX-FileContributor: Michele Di Giorgio, 2024

// Example usage of the following audio modes:
// - MM_MODE_A, Complete hardware mixing mode
// - MM_MODE_B, Interpolated mixing mode
// - MM_MODE_C, Extended mixing mode
//
// Check https://blocksds.github.io/docs/maxmod/group__nds__arm9__init.html
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

int main(int argc, char **argv)
{
    consoleDemoInit();

    mmInitDefaultMem((mm_addr)soundbank_bin);

    mmLoad(MOD_PARALLAX_80599);

    printf("maxmod audio modes example\n");
    printf("==========================\n");
    printf("\n");
    printf("A: Hardware mixing mode\n");
    printf("B: Interpolation mixing mode\n");
    printf("Y: Extended mixing\n");
    printf("\n");
    printf("START: Return to loader\n");

    soundEnable();

    mmStart(MOD_PARALLAX_80599, MM_PLAY_LOOP);

    while (1) {
        swiWaitForVBlank();

        scanKeys();

        uint16_t keys_down = keysDown();

        if (keys_down & KEY_START)
            break;

        if (keys_down & KEY_A) {
            mmSelectMode(MM_MODE_A);
        }
        if (keys_down & KEY_B) {
            mmSelectMode(MM_MODE_B);
        }
        if (keys_down & KEY_Y) {
            mmSelectMode(MM_MODE_C);
        }
    }

    mmStop();

    soundDisable();

    return 0;
}
