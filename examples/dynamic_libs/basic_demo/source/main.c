// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

#include <dlfcn.h>
#include <stdio.h>

#include <filesystem.h>
#include <nds.h>

void wait_forever(void)
{
    while (1)
        swiWaitForVBlank();
}

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

    // Use bright green

    printf("Loading library...\n");

    void *h = dlopen("dsl/calculator.dsl", RTLD_NOW | RTLD_LOCAL);
    const char *err = dlerror();
    if (err != NULL)
    {
        printf("dlopen(): %s\n", err);
        wait_forever();
    }

    printf("Resolving public functions...\n");
    printf("\n");

    fnptr1int *operation_set = dlsym(h, "operation_set");
    printf("operation_set: %p\n", operation_set);
    err = dlerror();
    if (err != NULL)
    {
        printf("dlsym(operation_set): %s\n", err);
        wait_forever();
    }

    fnptr2int *operation_run = dlsym(h, "operation_run");
    printf("operation_run: %p\n", operation_run);
    err = dlerror();
    if (err != NULL)
    {
        printf("dlsym(operation_run): %s\n", err);
        wait_forever();
    }
    fnptr1int *operation_arm = dlsym(h, "operation_arm");
    printf("operation_arm: %p\n", operation_arm);
    err = dlerror();
    if (err != NULL)
    {
        printf("dlsym(operation_arm): %s\n", err);
        wait_forever();
    }
    printf("\n");

    printf("Using library functions...\n");
    printf("\n");
    operation_set(0);
    printf("123 + 456 = %d\n", operation_run(123, 456));
    operation_set(1);
    printf("123 - 456 = %d\n", operation_run(123, 456));
    operation_set(2);
    printf("123 * 456 = %d\n", operation_run(123, 456));
    operation_set(3);
    printf("900 / 70 = %d\n", operation_run(900, 70));
    operation_set(4);
    printf("arr1[2] = %d\n", operation_run(2, 0));
    printf("arr1[3] = %d\n", operation_run(3, 0));
    operation_set(5);
    printf("arr2[2] = 0x%X\n", operation_run(2, 0));
    printf("arr2[3] = 0x%X\n", operation_run(3, 0));
    printf("\n");

    consoleSelect(&bottomScreen);

    printf("Calling operation_arm(5)...\n");
    int res = operation_arm(5);
    printf("Result = %d\n", res);
    printf("\n");

    // This should fail
    printf("Resolving private symbol...\n");
    void *ptr = dlsym(h, "op_add");
    printf("dlsym(op_add): %p\n", ptr);
    if (ptr != NULL)
    {
        printf("This should have failed!");
        wait_forever();
    }
    err = dlerror();
    if (err == NULL)
    {
        printf("This should have returned an error!\n");
        wait_forever();
    }
    printf("\n");

    printf("Unloading library...\n");

    dlclose(h);
    err = dlerror();
    if (err != NULL)
    {
        printf("dlclose(): %s\n", err);
        wait_forever();
    }

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
