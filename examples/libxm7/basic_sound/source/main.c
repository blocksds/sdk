// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <stdbool.h>
#include <stdio.h>

#include <libxm7.h>
#include <nds.h>

// Title: haen pyykit
// Author: Lasse
// Source: Obtained from the original LibXM7 example by sverx.
#include <lasse_haen_pyykit_xm_bin.h>

// Title: Parallax Glacier
// Author: Raina
// Source http://modarchive.org/index.php?request=view_by_moduleid&query=163194
#include <parallax_80599_xm_bin.h>

// Assign FIFO_USER_07 channel to LibXM7
#define FIFO_LIBXM7 FIFO_USER_07

void song_start(XM7_ModuleManager_Type *module)
{
    fifoSendValue32(FIFO_LIBXM7, (u32)module);
}

void song_stop(void)
{
    fifoSendValue32(FIFO_LIBXM7, 0);
}

// You can also allocate this with malloc()
static XM7_ModuleManager_Type module[2];

int main(int argc, char **argv)
{
    consoleDemoInit();

    printf("LibXM7 example\n");
    printf("==============\n");
    printf("\n");
    printf("X: haen pyykit by Lasse\n");
    printf("Y: Parallax Glacier by Raina\n");
    printf("\n");
    printf("B: Stop song\n");
    printf("\n");
    printf("START: Return to loader\n");

    bool songs_loaded = true;

    u16 res;

    res = XM7_LoadXM(&module[0], lasse_haen_pyykit_xm_bin);
    if (res != 0)
    {
        printf("LibXM7 error (module 0): 0x%04x\n", res);
        songs_loaded = false;
    }

    res = XM7_LoadXM(&module[1], parallax_80599_xm_bin);
    if (res != 0)
    {
        printf("LibXM7 error (module 1): 0x%04x\n", res);
        songs_loaded = false;
    }

    // Depending on the MOD or XM file you're playing, you may need to adjust
    // the replay style. Check the documentation for more details.
    //XM7_SetReplayStyle(&modules[0], XM7_REPLAY_STYLE_MOD_PLAYER);

    // Ensure that the ARM7 can see the LibXM7 initialized data
    DC_FlushAll();

    soundEnable();

    bool playing = false;

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint16_t keys_down = keysDown();

        if (songs_loaded)
        {
            if (keys_down & KEY_B)
            {
                if (playing)
                {
                    song_stop();
                    playing = false;
                }
            }

            if (keys_down & KEY_X)
            {
                if (playing)
                    song_stop();

                song_start(&module[0]);
                playing = true;
            }
            if (keys_down & KEY_Y)
            {
                if (playing)
                    song_stop();

                song_start(&module[1]);
                playing = true;
            }
        }

        if (keys_down & KEY_START)
            break;
    }

    XM7_UnloadXM(&module[0]);
    XM7_UnloadXM(&module[1]);

    soundDisable();

    return 0;
}
