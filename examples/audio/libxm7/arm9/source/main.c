// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <nds.h>
#include <stdbool.h>
#include <stdio.h>

#include <libxm7.h>

// XM module by Lasse. Obtained from the original libxm7 example by sverx
#include <lasse_haen_pyykit_xm_bin.h>

// Parallax Glacier by Raina:
// http://modarchive.org/index.php?request=view_by_moduleid&query=163194
#include <parallax_80599_xm_bin.h>

// Assign FIFO_USER_07 channel to libxm7
#define FIFO_XM7 (FIFO_USER_07)

void song_start(XM7_ModuleManager_Type *module)
{
    fifoSendValue32(FIFO_XM7, (u32)module);
}

void song_stop(void)
{
    fifoSendValue32(FIFO_XM7, 0);
}

// You can also allocate this with malloc()
static XM7_ModuleManager_Type module[2];

int main(int argc, char **argv)
{
    consoleDemoInit();

    printf("libXM7 example\n");
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

    res = XM7_LoadXM(&module[0],
                     (XM7_XMModuleHeader_Type *)lasse_haen_pyykit_xm_bin);
    if (res != 0)
    {
        printf("libxm7 error (module 0): 0x%04x\n", res);
        songs_loaded = false;
    }

    res = XM7_LoadXM(&module[1],
                     (XM7_XMModuleHeader_Type *)parallax_80599_xm_bin);
    if (res != 0)
    {
        printf("libxm7 error (module 1): 0x%04x\n", res);
        songs_loaded = false;
    }

    // Ensure that the ARM7 can see the libxm7 initialized data
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
