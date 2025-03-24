// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

#include <dirent.h>
#include <dlfcn.h>
#include <stdio.h>
#include <time.h>

#include <filesystem.h>
#include <nds.h>

// -----------------------------------------------

void wait_forever(void)
{
    while (1)
        swiWaitForVBlank();
}

// -----------------------------------------------

typedef int (*ptr_printf)(const char *fmt, ...);
typedef void *(*ptr_malloc)(size_t);
typedef void (*ptr_free)(void *);
typedef time_t (*ptr_time)(time_t *);

typedef void (PLUGIN_SetInterfacePrintf)(ptr_printf);
typedef void (PLUGIN_SetInterfaceMalloc)(ptr_malloc, ptr_free);
typedef void (PLUGIN_SetInterfaceTime)(ptr_time);
typedef int (PLUGIN_Run)(int);

#define MAX_PLUGINS 5

void *plugin_handles[MAX_PLUGINS];
PLUGIN_Run *plugin_run[MAX_PLUGINS];
int number_of_plugins = 0;

void load_plugin(int index, const char *path)
{
    printf("Loading plugin: %s\n", path);

    void *h = dlopen(path, RTLD_NOW | RTLD_LOCAL);
    const char *err = dlerror();
    if (err != NULL)
    {
        printf("dlopen(%s): %s\n", path, err);
        wait_forever();
    }

    // Provide as many interfaces as the plugin requires.

    {
        PLUGIN_SetInterfacePrintf *setifprintf = dlsym(h, "PLUGIN_SetInterfacePrintf");
        printf("PLUGIN_SetInterfacePrintf: %p\n", setifprintf);
        if (setifprintf != NULL)
        {
            setifprintf(printf);
        }
    }

    {
        PLUGIN_SetInterfaceMalloc *setifmalloc = dlsym(h, "PLUGIN_SetInterfaceMalloc");
        printf("PLUGIN_SetInterfaceMalloc: %p\n", setifmalloc);
        if (setifmalloc != NULL)
        {
            setifmalloc(malloc, free);
        }
    }

    {
        PLUGIN_SetInterfaceTime *setiftime = dlsym(h, "PLUGIN_SetInterfaceTime");
        printf("PLUGIN_SetInterfaceTime: %p\n", setiftime);
        if (setiftime != NULL)
        {
            setiftime(time);
        }
    }

    // PLUGIN_Run() is mandatory in all plugins, fail if it isn't found.

    {
        plugin_run[index] = dlsym(h, "PLUGIN_Run");
        printf("PLUGIN_Run: %p\n", plugin_run[index]);
        err = dlerror();
        if (err != NULL)
        {
            printf("dlsym(PLUGIN_Run): %s\n", err);
            wait_forever();
        }
    }

    plugin_handles[index] = h;
}

void load_all_plugins(const char *folder)
{
    if (chdir(folder) != 0)
    {
        perror("chdir()");
        return;
    }

    DIR *dirp = opendir(".");
    if (dirp == NULL)
    {
        perror("opendir");
        return;
    }

    number_of_plugins = 0;

    while (1)
    {
        struct dirent *cur = readdir(dirp);
        if (cur == NULL)
            break;

        if (strlen(cur->d_name) == 0)
            break;

        if (cur->d_type == DT_REG)
        {
            load_plugin(number_of_plugins, cur->d_name);

            number_of_plugins++;
            if (number_of_plugins == MAX_PLUGINS)
                break;
        }
    }

    closedir(dirp);
}

void free_all_plugins(void)
{
    for (int i = 0; i < number_of_plugins; i++)
    {
        dlclose(plugin_handles[i]);
        const char *err = dlerror();
        if (err != NULL)
        {
            printf("dlclose(%d): %s\n", i, err);
            wait_forever();
        }

        plugin_run[i] = NULL;
    }

    number_of_plugins = 0;
}

// -----------------------------------------------

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

    // Load all plugins from the "/dsl" folder in the SD card
    load_all_plugins("dsl");

    printf("Press START to exit to loader\n");

    consoleSelect(&bottomScreen);

    int iteration = 0;

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        if (keysHeld() & KEY_START)
            break;

        printf("Iteration: %d\n", iteration);

        for (int i = 0; i < number_of_plugins; i++)
            plugin_run[i](iteration);

        iteration++;
    }

    free_all_plugins();

    return 0;
}
