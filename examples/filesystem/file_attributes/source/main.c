// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023-2024

#include <dirent.h>

#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>

#include <fat.h>
#include <filesystem.h>
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

void print_attributes(const char *path)
{
    printf("Path: %s\n", path);

    int attributes = FAT_getAttr(path);
    if (attributes == -1)
    {
        printf("Failed to get attributes\n");
        return;
    }

    printf("Attributes:\n");
    printf("    %s %s %s %s %s %s\n",
        attributes & ATTR_ARCHIVE ? "ARC" : "-",
        attributes & ATTR_DIRECTORY ? "DIR" : "-",
        attributes & ATTR_VOLUME ? "VOL" : "-",
        attributes & ATTR_SYSTEM ? "SYS" : "-",
        attributes & ATTR_HIDDEN ? "HID" : "-",
        attributes & ATTR_READONLY ? "RO" : "-");
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

    print_attributes("nitro:/fstest");

    printf("\n");

    print_attributes("nitro:/fstest/f1.txt");

    if (FAT_setAttr("nitro:/fstest/f1.txt", ATTR_HIDDEN) != -1)
        printf("FAT_setAttr() should have failed!\n");

    wait_press_button_a();

    printf("\x1b[2J"); // Clear console

    printf("FAT (DLDI)\n");
    printf("==========\n");
    printf("\n");

    print_attributes("fat:/fstest");

    printf("\n");

    print_attributes("fat:/fstest/f1.txt");

    printf("\n");

    if (FAT_setAttr("fat:/fstest/f1.txt", ATTR_HIDDEN) == -1)
        perror("FAT_setAttr");

    print_attributes("fat:/fstest/f1.txt");

    printf("\n");

    if (FAT_setAttr("fat:/fstest/f1.txt", 0) == -1)
        perror("FAT_setAttr");

    print_attributes("fat:/fstest/f1.txt");

    printf("\n");

    wait_press_button_a();

    printf("\x1b[2J"); // Clear console

    printf("SD (DSi)\n");
    printf("========\n");
    printf("\n");

    print_attributes("sd:/fstest");

    printf("\n");

    print_attributes("sd:/fstest/f1.txt");

    printf("\n");

    if (FAT_setAttr("sd:/fstest/f1.txt", ATTR_HIDDEN) == -1)
        perror("FAT_setAttr");

    print_attributes("sd:/fstest/f1.txt");

    printf("\n");

    if (FAT_setAttr("sd:/fstest/f1.txt", 0) == -1)
        perror("FAT_setAttr");

    print_attributes("sd:/fstest/f1.txt");

    printf("\n");

    wait_press_button_a();

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
