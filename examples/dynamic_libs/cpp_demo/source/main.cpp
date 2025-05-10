// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

#include <errno.h>
#include <dlfcn.h>
#include <stdio.h>

#include <filesystem.h>
#include <nds.h>

void wait_forever(void)
{
    while (1)
        swiWaitForVBlank();
}

typedef void (fnptrvoid)(void);
typedef int (fnptr1int)(int);
typedef int (fnptr2int)(int, int);

int main(int argc, char **argv)
{
    defaultExceptionHandler();

    PrintConsole topScreen;
    PrintConsole bottomScreen;

    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(&topScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    consoleInit(&bottomScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);

    consoleSelect(&topScreen);

    bool init_ok = nitroFSInit(NULL);
    if (!init_ok)
    {
        perror("nitroFSInit()");
        wait_forever();
    }

    printf("[*] Loading library...\n");
    printf("\n");

    void *h = dlopen("dsl/test.dsl", RTLD_NOW | RTLD_LOCAL);
    const char *err = dlerror();
    if (err != NULL)
    {
        printf("dlopen(): %s\n", err);
        wait_forever();
    }
    printf("\n");

    printf("[*] Resolving functions...\n");
    printf("\n");

    VoidFn my_print = (VoidFn)dlsym(h, "_Z10print_textv");
    printf("_Z10print_textv: %p\n", my_print);
    err = dlerror();
    if (err != NULL)
    {
        printf("dlsym(_Z10print_textv): %s\n", err);
        wait_forever();
    }

    printf("\n");

    printf("[*] Using library functions...\n");
    printf("\n");

    my_print();

    printf("\n");
    printf("[*] Unloading library...\n");
    printf("\n");

    dlclose(h);
    err = dlerror();
    if (err != NULL)
    {
        printf("dlclose(): %s\n", err);
        wait_forever();
    }

    consoleSelect(&bottomScreen);

    printf("Press START to exit to loader\n");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        if (keysHeld() & KEY_START)
            break;
    }

    return 0;
}
