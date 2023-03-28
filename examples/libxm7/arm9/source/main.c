// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <nds.h>
#include <stdbool.h>
#include <stdio.h>

#include <libxm7.h>

// XM module by Lasse. Obtained from the original libxm7 example by sverx
#include <lasse_haen_pyykit_xm_bin.h>

// Assign FIFO_USER_07 channel to libxm7
#define FIFO_XM7    (FIFO_USER_07)

void song_start(XM7_ModuleManager_Type *module)
{
    fifoSendValue32(FIFO_XM7, (u32)module);
}

void song_stop(void)
{
    fifoSendValue32(FIFO_XM7, 0);
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    printf("libXM7 example\n");
    printf("\n");
    printf("A:     Start song\n");
    printf("B:     Stop song\n");
    printf("START: Return to loader\n");

    bool libxm7_enabled = true;

    XM7_ModuleManager_Type *module = malloc(sizeof(XM7_ModuleManager_Type));
    if (module == NULL)
    {
        printf("malloc() error\n");
        libxm7_enabled = false;
    }

    u16 res = XM7_LoadXM(module, (XM7_XMModuleHeader_Type *)lasse_haen_pyykit_xm_bin);
    if (res != 0)
    {
        printf("libxm7 error: 0x%04x\n", res);
        libxm7_enabled = false;
    }

    // Ensure that the ARM7 can see the libxm7 initialized data
    DC_FlushAll();

    soundEnable();

    bool playing = false;

    while(1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint16_t keys_down = keysDown();

        if (libxm7_enabled)
        {
            if (playing)
            {
                if (keys_down & KEY_B)
                {
                    song_stop();
                    playing = false;
                }
            }
            else
            {
                if (keys_down & KEY_A)
                {
                    song_start(module);
                    playing = true;
                }
            }

        }

        if (keys_down & KEY_START)
            break;
    }

    XM7_UnloadXM(module);

    soundDisable();

    return 0;
}
