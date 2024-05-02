// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

// This test calls realpath() in many situations to test that it behaves as
// expected.

#include <errno.h>
#include <stdio.h>
#include <limits.h>

#include <fat.h>
#include <filesystem.h>
#include <nds.h>

#include <nds/arm9/dldi.h>

PrintConsole topScreen;
PrintConsole bottomScreen;

void wait_forever(void)
{
    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        if (keysHeld() & KEY_START)
            exit(0);
    }
}

void print_cwd(void)
{
    char *cwd = getcwd(NULL, 0);
    printf("cwd: %s\n", cwd);
    free(cwd);
}

void realpath_test_success(const char *test_path, const char *expected)
{
    char *result = realpath(test_path, NULL);
    if (strcmp(result, expected) != 0)
    {
        printf("FAIL\n");
        printf("  realpath(%s)\n", test_path);
        printf("  result:   %s\n", result);
        printf("  expected: %s\n", expected);
        printf("\n");

        wait_forever();
    }
    free(result);
}

void realpath_test_error(const char *test_path, int expected_errno)
{
    char *result = realpath(test_path, NULL);
    if (result != NULL)
    {
        printf("FAIL\n");
        printf("  realpath(%s)\n", test_path);
        printf("  result:   %s\n", result);
        printf("  expected: NULL\n");
        printf("\n");

        wait_forever();
    }

    if (errno != expected_errno)
    {
        printf("FAIL\n");
        printf("  realpath(%s)\n", test_path);
        printf("  errno:    %d\n", errno);
        printf("  expected: %d\n", expected_errno);
        printf("\n");

        wait_forever();
    }
}

char path_too_long[PATH_MAX * 2];

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

    // Do most of the tests with nitroFS so that they work in all emulators
    // --------------------------------------------------------------------

    printf("\x1b[2J"); // Clear console

    printf("[*] NitroFS\n");
    printf("\n");

    chdir("nitro:/");
    print_cwd();
    printf("\n");

    realpath_test_success("/test/ab", "nitro:/test/ab");
    realpath_test_success("/../test/ab", "nitro:/test/ab");
    realpath_test_success("nitro:/test/abc", "nitro:/test/abc");
    realpath_test_success("nitro:/../test/abc", "nitro:/test/abc");

    realpath_test_success("test/a", "nitro:/test/a");
    realpath_test_success("../ab", "nitro:/ab");
    realpath_test_success(".././abc", "nitro:/abc");
    realpath_test_success("test/..//./a", "nitro:/a");
    realpath_test_success(".././test/../.././abc", "nitro:/abc");

    chdir("fstest");
    print_cwd();
    printf("\n");

    realpath_test_success("/test/ab", "nitro:/test/ab");
    realpath_test_success("/../test/ab", "nitro:/test/ab");
    realpath_test_success("nitro:/test/abc", "nitro:/test/abc");
    realpath_test_success("nitro:/../test/abc", "nitro:/test/abc");

    realpath_test_success("test/a", "nitro:/fstest/test/a");
    realpath_test_success("test", "nitro:/fstest/test");
    realpath_test_success(".", "nitro:/fstest");
    realpath_test_success("a", "nitro:/fstest/a");
    realpath_test_success("..", "nitro:/");
    realpath_test_success(".a", "nitro:/fstest/.a");
    realpath_test_success("...", "nitro:/fstest/...");
    realpath_test_success("..a", "nitro:/fstest/..a");

    // Test that realpath() uses our provided buffer instead of allocating one
    // when we pass a pointer instead of NULL
    {
        char *result = realpath("/test/path", path_too_long);
        if (result != path_too_long)
        {
            printf("FAIL\n");
            printf("  realpath allocated a new buffer\n");
            printf("\n");

            wait_forever();
        }
        if (strcmp(path_too_long, "nitro:/test/path") != 0)
        {
            printf("FAIL\n");
            printf("  realpath didn't write to our buffer\n");
            printf("  result:   %s\n", result);
            printf("  expected: nitro:/test/path\n");
            printf("\n");

            wait_forever();
        }
    }

    // TODO: Okay... so this is the expected behaviour of realpath() the way it
    // is implemented in libnds. It is an invalid drive name, but realpath()
    // doesn't check if the drive makes sense. If the drive name is provided,
    // realpath() uses it as it is provided.
    realpath_test_success(":/", ":/");

    // NULL path
    realpath_test_error(NULL, EINVAL);

    // Empty path
    realpath_test_error("", ENOENT);

    // Path that is too long
    {
        memset(path_too_long, 'c', sizeof(path_too_long));
        path_too_long[sizeof(path_too_long) - 1] = '\0';
        realpath_test_error(path_too_long, ENAMETOOLONG);
    }

    // Path that is formed from a string that is too long, but it has enough
    // ".." to make it fit after they are evaluated.
    {
        path_too_long[0] = '\0';
        for (size_t i = 0; i <= sizeof(path_too_long); )
        {
            const char *str = "this_is_a_folder_name/../";
            strcat(path_too_long, str);
            i += strlen(str);
        }
        realpath_test_success(path_too_long, "nitro:/fstest");
    }

    // Run realpath() when there isn't enough RAM to allocate PATH_MAX bytes
    {
        // For this we need to fill the heap (as many chunks as needed)

        char *ptr[10] = { 0 };
        int ptr_last = 0;

        size_t ram_size = (isDSiMode() ? 16 : 4) * 1024 * 1024;
        size_t remaining_size = ram_size;

        while (1)
        {
            size_t size = remaining_size;

            while (1)
            {
                ptr[ptr_last] = malloc(size);
                if (ptr[ptr_last] != NULL)
                {
                    remaining_size -= size;
                    break;
                }

                size -= 4;
                if (size == 0)
                    goto end_fill_heap;
            }

            ptr_last++;

            if (ptr_last == 10)
            {
                printf("FAIL: Can't fill heap");
                wait_forever();
            }
        }
end_fill_heap:

        printf("Allocated: %d chunks\n", ptr_last);

        realpath_test_error("test", ENOMEM);
        realpath_test_error("test/string", ENOMEM);

        for (int i = 0; i < 10; i++)
            free(ptr[i]); // It is ok to free() a NULL pointer
    }

    // Make sure that after freeing the heap we can run the funciton again
    realpath_test_success("test/string", "nitro:/fstest/test/string");

    printf("\n");

    // Now, a few simple tests with FAT and DSi SD
    // -------------------------------------------

    printf("[*] FAT (DLDI)\n");
    printf("\n");

    if (chdir("fat:/") != 0)
    {
        printf("fat:/ not available\n");
        printf("\n");
    }
    else
    {
        print_cwd();
        printf("\n");

        realpath_test_success("/test/ab", "fat:/test/ab");
        realpath_test_success("/../test/ab", "fat:/test/ab");
        realpath_test_success("fat:/test/abc", "fat:/test/abc");
        realpath_test_success("fat:/../test/abc", "fat:/test/abc");

        realpath_test_success("test/a", "fat:/test/a");
        realpath_test_success("../ab", "fat:/ab");
        realpath_test_success(".././abc", "fat:/abc");
        realpath_test_success("test/..//./a", "fat:/a");
        realpath_test_success(".././test/../.././abc", "fat:/abc");

        if (chdir("fstest") != 0)
        {
            printf("fstest not available\n");
            printf("\n");
        }
        else
        {
            print_cwd();
            printf("\n");

            realpath_test_success("/test/ab", "fat:/test/ab");
            realpath_test_success("/../test/ab", "fat:/test/ab");
            realpath_test_success("fat:/test/abc", "fat:/test/abc");
            realpath_test_success("fat:/../test/abc", "fat:/test/abc");

            realpath_test_success("test/a", "fat:/fstest/test/a");
            realpath_test_success("test", "fat:/fstest/test");
        }
    }

    printf("[*] SD (DSi)\n");
    printf("\n");

    if (chdir("sd:/") != 0)
    {
        printf("sd:/ not available\n");
        printf("\n");
    }
    else
    {
        print_cwd();
        printf("\n");

        realpath_test_success("/test/ab", "sd:/test/ab");
        realpath_test_success("/../test/ab", "sd:/test/ab");
        realpath_test_success("sd:/test/abc", "sd:/test/abc");
        realpath_test_success("sd:/../test/abc", "sd:/test/abc");

        realpath_test_success("test/a", "sd:/test/a");
        realpath_test_success("../ab", "sd:/ab");
        realpath_test_success(".././abc", "sd:/abc");
        realpath_test_success("test/..//./a", "sd:/a");
        realpath_test_success(".././test/../.././abc", "sd:/abc");

        if (chdir("fstest") != 0)
        {
            printf("fstest not available\n");
            printf("\n");
        }
        else
        {
            print_cwd();
            printf("\n");

            realpath_test_success("/test/ab", "sd:/test/ab");
            realpath_test_success("/../test/ab", "sd:/test/ab");
            realpath_test_success("sd:/test/abc", "sd:/test/abc");
            realpath_test_success("sd:/../test/abc", "sd:/test/abc");

            realpath_test_success("test/a", "sd:/fstest/test/a");
            realpath_test_success("test", "sd:/fstest/test");
        }
    }

    printf("Tests finished!\n");
    printf("\n");
    printf("Press START to exit to loader\n");

    wait_forever();

    return 0;
}
