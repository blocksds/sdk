// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <dirent.h>

#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>

#include <fatfs.h>
#include <nds.h>

#include <nds/arm9/dldi.h>

PrintConsole topScreen;
PrintConsole bottomScreen;

void wait_press_button_a(void)
{
    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        if (keysDown() & KEY_A)
            break;
    }
}

void wait_forever(void)
{
    while (1)
        swiWaitForVBlank();
}

void dir_list(void)
{
    //printf("\x1b[2J"); // Clear console

    char *cwd = getcwd(NULL, 0);
    printf("path: %s\n", cwd);
    free(cwd);
    printf("\n");

    DIR *dirp = opendir(".");
    if (dirp == NULL)
    {
        perror("opendir");
        wait_forever();
    }

    int num_entries = 0;
    while (1)
    {
        struct dirent *cur = readdir(dirp);
        if (cur == NULL)
            break;

        if (strlen(cur->d_name) == 0)
            break;

        num_entries++;
    }

    rewinddir(dirp);

    int total_printed = 0;

    while (1)
    {
        struct dirent *cur = readdir(dirp);
        if (cur == NULL)
            break;

        if (strlen(cur->d_name) == 0)
            break;

        int index = telldir(dirp);
        printf("%d - %s%s\n", index, cur->d_name,
               (cur->d_type == DT_DIR) ? "/" : " ");

        total_printed++;
        if (total_printed == 10)
            break;
    }

    closedir(dirp);

    printf("\x1b[23;0HNum entries: %d ", num_entries);

    wait_press_button_a();

    printf("\x1b[2J"); // Clear console
}

int main(int argc, char **argv)
{
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(&topScreen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    consoleInit(&bottomScreen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);

    consoleSelect(&topScreen);

    printf("\x1b[2J"); // Clear console

    printf("DLDI name:\n");
    printf("%s\n", io_dldi_data->friendlyName);
    printf("\n");
    printf("DSi mode: %d\n", isDSiMode());
    printf("\n");
    printf("argc: %d\n", argc);
    if (argc > 0)
        printf("argv[0]: %s\n", argv[0]);
    printf("\n");

    // Initialize FAT and NitroFS independently in case this ROM is running in
    // an emulator. In that case, NitroFS will work with card commands, so FAT
    // would neve be initialized. On a DS flashcard or a DSi FAT would be
    // initialized by nitroFSInit(), but it's ok if we have initialized it
    // before.

    bool init_ok = fatInitDefault();
    if (!init_ok)
    {
        perror("fatInitDefault()");
        // We may be running in an emulator, don't hang the execution
    }

    init_ok = nitroFSInit(NULL);
    if (!init_ok)
    {
        perror("nitroFSInit()");
        wait_forever();
    }

    consoleSelect(&bottomScreen);

    // List the contents in a folder in NitroFS

    printf("\x1b[2J"); // Clear console

    printf("NitroFS\n");
    printf("=======\n");
    printf("\n");

    if (chdir("nitro:/fstest") == 0)
    {
        dir_list();
    }
    else
    {
        printf("Failed to chdir()\n");
        printf("\n");
        wait_press_button_a();
    }

    // List the contents in the root folder of the FAT filesystem (flashcard)

    printf("\x1b[2J"); // Clear console

    printf("FAT (DLDI)\n");
    printf("==========\n");
    printf("\n");

    if (chdir("fat:/") == 0)
    {
        dir_list();
    }
    else
    {
        printf("Failed to chdir()\n");
        printf("\n");
        wait_press_button_a();
    }

    // List the contents in the root folder of the DSi SD card

    printf("\x1b[2J"); // Clear console

    printf("SD (DSi)\n");
    printf("========\n");
    printf("\n");

    if (chdir("sd:/") == 0)
    {
        dir_list();
    }
    else
    {
        printf("Failed to chdir()\n");
        printf("\n");
        wait_press_button_a();
    }

    // Clear console
    printf("\x1b[2J");
    printf("Tests finished!\n");
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

    return 0;
}
