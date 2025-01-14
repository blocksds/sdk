// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023-2024

#include <stdbool.h>
#include <stdio.h>

#include <filesystem.h>
#include <libxm7.h>
#include <nds.h>

// List of songs used in this example:
//
// kaos_och_dekadens.mod
// ---------------------
//
// - Title: Kaos och Dekadens
// - Author: Nightbeat
// - Source: https://modarchive.org/index.php?request=view_by_moduleid&query=113319
//
// lasse_haen_pyykit.xm
// --------------------
//
// - Title: haen pyykit
// - Author: Lasse
// - Source: Obtained from the original LibXM7 example by sverx.
//
// nb_essentials.mod
// -----------------
//
// - Title: Essentials Intact
// - Author: Nightbeat
// - Source: https://modarchive.org/index.php?request=view_by_moduleid&query=49587
//
// nb_roots.mod
// ------------
//
// - Title: Roots
// - Author: Nightbeat
// - Source: https://modarchive.org/index.php?request=view_by_moduleid&query=49644
//
// parallax_80599.xm
// -----------------
//
// - Title: Parallax Glacier
// - Author: Raina
// - Source http://modarchive.org/index.php?request=view_by_moduleid&query=163194
//
// secret_message.mod
// ------------------
//
// - Title: Secret Message
// - Author: Frequent/Ephidrena
// - Source: https://modarchive.org/index.php?request=view_by_moduleid&query=96883

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

static void *file_buffer = NULL;
static XM7_ModuleManager_Type *module_manager = NULL;

void song_nitrofs_stop_and_free(void)
{
    if (file_buffer == NULL)
        return;

    song_stop();

    // Wait until the player has stopped to free the data
    swiWaitForVBlank();

    XM7_UnloadXM(module_manager);

    free(module_manager);
    free(file_buffer);

    file_buffer = NULL;
}

void song_nitrofs_load_and_play(const char *path)
{
    if (file_buffer)
        song_nitrofs_stop_and_free();

    file_buffer = file_load(path);
    if (file_buffer == NULL)
        return;

    module_manager = calloc(1, sizeof(XM7_ModuleManager_Type));
    if (module_manager == NULL)
    {
        free(file_buffer);
        return;
    }

    u16 res = 0xFFFF;

    if (strcmp(path + strlen(path) - 4, ".mod") == 0)
        res = XM7_LoadMOD(module_manager, file_buffer);
    else if (strcmp(path + strlen(path) - 3, ".xm") == 0)
        res = XM7_LoadXM(module_manager, file_buffer);

    // Depending on the MOD or XM file you're playing, you may need to adjust
    // the replay style. Check the documentation for more details.
    //XM7_SetReplayStyle(module_manager, XM7_REPLAY_STYLE_MOD_PLAYER);

    if (res != 0)
    {
        printf("LibXM7 error (module 0): 0x%04x\n", res);
        free(file_buffer);
        free(module_manager);
        return;
    }

    // Ensure that the ARM7 can see the file and the LibXM7 struct
    DC_FlushAll();

    song_start(module_manager);
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    printf("LibXM7 example\n");
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

    soundEnable();

    printf("A:       Start song\n");
    printf("B:       Stop song\n");
    printf("UP/DOWN: Select song\n");
    printf("\n");
    printf("START: Return to loader\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("Available songs:\n");
    printf("\n");

#define NUM_SONGS 6

    const char *songs[NUM_SONGS] = {
        "kaos_och_dekadens.mod",
        "lasse_haen_pyykit.xm",
        "nb_essentials.mod",
        "nb_roots.mod",
        "parallax_80599.xm",
        "secret_message.mod",
    };

    int selected_song = 0;

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();
        uint16_t keys_down = keysDown();

        // Print list of songs
        consoleSetCursor(NULL, 0, 15);
        for (int i = 0; i < NUM_SONGS; i++)
            printf("%c %s\n", selected_song == i ? '>' : ' ', songs[i]);

        if (keys_down & KEY_B)
            song_nitrofs_stop_and_free();

        if (keys_down & KEY_A)
            song_nitrofs_load_and_play(songs[selected_song]);

        if (keys_down & KEY_DOWN)
        {
            selected_song++;
            if (selected_song >= NUM_SONGS)
                selected_song = 0;
        }
        if (keys_down & KEY_UP)
        {
            selected_song--;
            if (selected_song < 0)
                selected_song = NUM_SONGS - 1;
        }

        if (keys_down & KEY_START)
        {
            song_nitrofs_stop_and_free();
            break;
        }
    }

    soundDisable();

    return 0;
}
