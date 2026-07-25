// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023-2026

#include <fcntl.h>
#include <stdio.h>

#include <filesystem.h>
#include <nds.h>

#include <nds/arm9/dldi.h>

static PrintConsole topScreen;
static PrintConsole bottomScreen;

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

void fcat(const char *path)
{
    printf("cat: %s\n", path);

    int fd = open(path, O_RDONLY);
    if (fd < 0)
    {
        perror("open");
        wait_forever();
    }

    off_t size = lseek(fd, 0, SEEK_END);
    if (size == -1)
    {
        perror("lseek");
        wait_forever();
    }

    printf("Size: %ld bytes\n", size);

    off_t ret = lseek(fd, 0, SEEK_SET);
    if (ret == -1)
    {
        perror("lseek");
        wait_forever();
    }

    char *buffer = malloc(size + 1);
    if (buffer == NULL)
    {
        printf("malloc(): Not enough memory (%ld)\n", size);
        wait_forever();
    }

    ssize_t bytes = read(fd, buffer, size);
    if (bytes != size)
    {
        printf("read(%ld): %zu bytes read", size, bytes);
        wait_forever();
    }
    buffer[size - 1] = '\0';

    printf("[%s]\n", buffer);

    int res = close(fd);
    if (res != 0)
    {
        perror("close");
        wait_forever();
    }

    free(buffer);
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

    printf("DLDI name:\n");
    printf("%s\n", io_dldi_data->friendlyName);
    printf("\n");
    printf("DSi mode: %d\n", isDSiMode());
    printf("\n");
    printf("argc: %d\n", argc);
    if (argc > 0)
        printf("argv[0]: %s\n", argv[0]);
    printf("\n");

    // It isn't needed to call fatInitDefault() manually. If nitroFSInit detects
    // that the ROM is running in a flashcard or from the DSi internal SD slot,
    // it will call it internally.
    bool init_ok = nitroFSInit(NULL);
    if (!init_ok)
    {
        perror("nitroFSInit()");
        wait_forever();
    }

    consoleSelect(&bottomScreen);

    // Change the current working directory to the base directory used for
    // testing.

    chdir("nitro:/fstest");

    char *cwd = getcwd(NULL, 0);
    printf("Current dir: %s\n", cwd);
    free(cwd);
    wait_press_button_a();

    // Print contents of one file

    consoleClear();
    fcat("long_file.txt");
    wait_press_button_a();

    // Done!

    consoleClear();
    printf("All test passed!\n");
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
