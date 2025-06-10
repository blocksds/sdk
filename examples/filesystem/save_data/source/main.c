// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023-2025

// This example shows a very basic way of saving data from an application and
// loading it again when the application starts later. You can press all buttons
// in the console, the program will count how many time you've pressed them.
// When you exit with START+SELECT it will save the counts to the SD card right
// next to the NDS ROM. When the application starts again it will load the
// saved data and start with the previous count.

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>

#include <fat.h>
#include <nds.h>

#include <nds/arm9/dldi.h>

typedef struct {
    uint32_t key_press_count[15];
} game_state_t;

static game_state_t game_state;

static bool load_data(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL)
    {
        printf("Can't open file\n");
        return false;
    }

    if (fread(&game_state, sizeof(game_state), 1, f) != 1)
    {
        printf("Can't read file\n");
        return false;
    }

    if (fclose(f) != 0)
    {
        printf("Can't close file\n");
        return false;
    }

    return true;
}

static bool save_data(const char *path)
{
    FILE *f = fopen(path, "wb");
    if (f == NULL)
    {
        printf("Can't open file\n");
        return false;
    }

    if (fwrite(&game_state, sizeof(game_state), 1, f) != 1)
    {
        printf("Can't write file\n");
        return false;
    }

    if (fclose(f) != 0)
    {
        printf("Can't close file\n");
        return false;
    }

    return true;
}

static PrintConsole topScreen;
static PrintConsole bottomScreen;

int main(int argc, char **argv)
{
    char *save_path = NULL;

    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(&topScreen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    consoleInit(&bottomScreen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);

    consoleSelect(&topScreen);

    printf("DLDI name:\n");
    printf("%s\n", io_dldi_data->friendlyName);
    printf("\n");
    printf("DSi mode: %d\n", isDSiMode());
    printf("\n");
    printf("argc: %d\n", argc);
    if (argc > 0)
        printf("argv[0]: %s\n", argv[0]);
    printf("\n");

    // Initialize FAT so that we can read and write files in the SD card

    bool init_ok = fatInitDefault();
    if (!init_ok)
    {
        // We need FAT access for this example
        perror("fatInitDefault()");
        goto error;
    }

    if (argc == 0)
    {
        printf("Error: argc = 0\n");
        goto error;
    }

    if (argv[0] == NULL)
    {
        printf("Error: argv[0] = NULL\n");
        goto error;
    }

    if (strlen(argv[0]) < 4)
    {
        printf("Error: argv[0] is too short\n");
        goto error;
    }

    // Create a string to hold the path to the saved data.

    save_path = strdup(argv[0]);
    if (save_path == NULL)
    {
        printf("Not enough memory for strdup()\n");
        goto error;
    }

    // Replace the ".nds" extension by ".sav"

    size_t save_path_len = strlen(save_path);
    strcpy(&save_path[save_path_len - 3], "sav");

    printf("Save path: %s\n", save_path);

    // Try to load saved data

    if (load_data(save_path))
    {
        printf("Saved data loaded!\n");
    }
    else
    {
        printf("Can't load saved data!\n");
    }

    consoleSelect(&bottomScreen);

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();
        uint16_t keys_down = keysDown();

        for (int i = 0; i < 15; i++)
        {
            if (keys_down & BIT(i))
                game_state.key_press_count[i]++;
        }

        uint16_t keys_held = keysHeld();
        if ((keys_held & KEY_SELECT) && (keys_held & KEY_START))
            break;

        consoleClear();

        printf("SELECT + START: Save and exit\n");
        printf("\n");

        const char *key_name[15] = {
            "A", "B", "SELECT", "START", "RIGHT", "LEFT", "UP", "DOWN",
            "R", "L", "X", "Y", "TOUCH", "LID", "DEBUG"
        };

        for (int i = 0; i < 15; i++)
            printf("%s: %" PRIu32 "\n", key_name[i], game_state.key_press_count[i]);

    }

    consoleSelect(&topScreen);

    if (!save_data(save_path))
    {
        printf("Can't save data!\n");
        goto error;
    }

    free(save_path);

    return 0;

error:
    printf("\n");
    printf("Press START to exit to loader\n");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint32_t keys_down = keysDown();
        if (keys_down & KEY_START)
            break;
    }

    free(save_path);

    return 0;
}
