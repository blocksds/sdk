// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023
// SPDX-FileContributor: Adrian "asie" Siekierka, 2023

#include <dirent.h>

#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>

#include <fat.h>
#include <nds.h>

#include <nds/arm9/dldi.h>

PrintConsole topScreen;
PrintConsole bottomScreen;

void wait_press_button_a(void)
{
    while(1)
    {
        swiWaitForVBlank();

        scanKeys();

        if (keysDown() & KEY_A)
            break;
    }
}

void wait_forever(void)
{
    while(1)
        swiWaitForVBlank();
}

void dir_list(void)
{
    printf("\x1b[2J"); // Clear console

    printf("path: %s\n", getcwd(NULL, 0));
    printf("\n");

    struct dirent **list;
    int list_count = scandir(".", &list, NULL, alphasort);

    if (list_count < 0)
    {
        perror("scandir");
        wait_forever();
    }

    for (int i = 0; i < list_count; i++)
        printf("%d - %s%s\n", i, list[i]->d_name,
               (list[i]->d_type == DT_DIR) ? "/" : " ");

    printf("\x1b[23;0HNum entries: %d ", list_count);

    for (int i = 0; i < list_count; i++)
        free(list[i]);
    free(list);
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

    consoleSelect(&bottomScreen);

    bool init_ok = fatInitDefault();
    if (!init_ok)
    {
        perror("fatInitDefault()");
        wait_forever();
    }

    char *cwd = getcwd(NULL, 0);
    printf("Current dir: %s\n\n", cwd);
    free(cwd);

    dir_list();

    printf("\nPress START to exit to loader\n");

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
