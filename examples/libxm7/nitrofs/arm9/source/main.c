// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023-2024

#include <nds.h>
#include <stdbool.h>
#include <stdio.h>

#include <filesystem.h>
#include <libxm7.h>

void wait_forever(void)
{
    while (1)
    {
        swiWaitForVBlank();
        scanKeys();
        if (keysHeld() & KEY_START)
            exit(1);
    }
}

void *file_load(const char *path)
{
    FILE *f = fopen(path, "r");
    if (f == NULL)
    {
        perror("fopen");
        wait_forever();
    }

    int ret = fseek(f, 0, SEEK_END);
    if (ret != 0)
    {
        perror("fseek");
        wait_forever();
    }

    long size = ftell(f);
    if (size == -1)
    {
        perror("ftell");
        wait_forever();
    }

    rewind(f);

    void *buffer = malloc(size);
    if (buffer == NULL)
    {
        printf("malloc(): Not enough memory (%ld)\n", size);
        wait_forever();
    }

    size_t bytes = fread(buffer, 1, size, f);
    if (bytes != size)
    {
        printf("fread(%ld): %zu bytes read", size, bytes);
        wait_forever();
    }

    int res = fclose(f);
    if (res != 0)
    {
        perror("fclose");
        wait_forever();
    }

    return buffer;
}

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
    printf("Initializing NitroFS...\n");
    printf("\n");

    bool init_ok = nitroFSInit(NULL);
    if (!init_ok)
    {
        perror("nitroFSInit()");
        wait_forever();
    }

    printf("X: haen pyykit by Lasse\n");
    printf("Y: Parallax Glacier by Raina\n");
    printf("\n");
    printf("B: Stop song\n");
    printf("\n");
    printf("START: Return to loader\n");

    bool songs_loaded = true;

    // XM module by Lasse. Obtained from the original libxm7 example by sverx
    void *lasse = file_load("lasse_haen_pyykit.xm");

    // Parallax Glacier by Raina:
    // http://modarchive.org/index.php?request=view_by_moduleid&query=163194
    void *parallax = file_load("parallax_80599.xm");

    u16 res;

    res = XM7_LoadXM(&module[0], lasse);
    if (res != 0)
    {
        printf("libxm7 error (module 0): 0x%04x\n", res);
        songs_loaded = false;
    }

    res = XM7_LoadXM(&module[1], parallax);
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

    free(lasse);
    free(parallax);

    soundDisable();

    return 0;
}
