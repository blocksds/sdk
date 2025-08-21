// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <stdio.h>

#include <filesystem.h>
#include <nds.h>
#include <maxmod9.h>

__attribute__((noreturn)) void WaitLoop(void)
{
    printf("Press START to exit");
    while (1)
    {
        swiWaitForVBlank();
        scanKeys();
        if (keysHeld() & KEY_START)
            exit(0);
    }
}

bool file_load(const char *path, void **buffer, size_t *size)
{
    // Open the file in read binary mode
    FILE *f = fopen(path, "rb");
    if (f == NULL)
    {
        perror("fopen");
        return false;
    }

    // Move read cursor to the end of the file
    int ret = fseek(f, 0, SEEK_END);
    if (ret != 0)
    {
        perror("fseek");
        return false;
    }

    // Check position of the cursor (we're at the end, so this is the size)
    *size = ftell(f);
    if (*size == 0)
    {
        printf("Size is 0!");
        fclose(f);
        return false;
    }

    // Move cursor to the start of the file again
    rewind(f);

    // Allocate buffer to hold data
    *buffer = malloc(*size);
    if (*buffer == NULL)
    {
        printf("Not enought memory to load %s!", path);
        fclose(f);
        return false;
    }

    // Read all data into the buffer
    if (fread(*buffer, *size, 1, f) != 1)
    {
        perror("fread");
        fclose(f);
        free(*buffer);
        return false;
    }

    // Close file
    ret = fclose(f);
    if (ret != 0)
    {
        perror("fclose");
        free(*buffer);
        return false;
    }

    return true;
}

int main(int argc, char **argv)
{
    defaultExceptionHandler();

    // Setup sub screen for the text console
    consoleDemoInit();

    if (!nitroFSInit(NULL))
    {
        printf("nitroFSInit failed.\n");
        WaitLoop();
    }

    // Setup Maxmod
    mmInitNoSoundbank();

    void *module;
    size_t size;

    if (!file_load("nitro:/mas/music/joint_people_mod.mas", &module, &size))
        WaitLoop();

    // Flush the cache so that the ARM7 can see the module data. The ARM7 is the
    // one that will actually play the module.
    DC_FlushRange(module, size);

    mmPlayMAS((uintptr_t)module, MM_PLAY_LOOP, MM_MAIN);

    consoleClear();

    printf("Maxmod MAS demo\n");
    printf("===============\n");
    printf("\n");
    printf("START: Exit to loader\n");
    printf("\n");

    while (1)
    {
        // Synchronize game loop to the screen refresh
        swiWaitForVBlank();

        scanKeys();

        uint16_t keys = keysHeld();

        if (keys & KEY_START)
            break;
    }

    free(module);

    return 0;
}
