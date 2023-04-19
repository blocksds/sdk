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
    fflush(stdout);

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
    fflush(stdout);

    while(1)
        swiWaitForVBlank();
}

void dir_list(void)
{
    printf("\x1b[2J"); // Clear console

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
    }

    closedir(dirp);

    printf("\x1b[23;0HNum entries: %d ", num_entries);

    wait_press_button_a();

    printf("\x1b[2J"); // Clear console
}

void fcat(const char *path)
{
    printf("cat: %s\n", path);

    FILE *f = fopen(path, "r");
    if (f == NULL)
    {
        perror("fopen");
        wait_forever();
    }

    int ret = fseek(f, 0, SEEK_END);
    if (ret != 0)
    {
        perror("fseek");
        wait_forever();
    }

    long size = ftell(f);
    if (size == -1)
    {
        perror("ftell");
        wait_forever();
    }

    printf("Size: %ld bytes\n", size);

    rewind(f);

    char *buffer = malloc(size + 1);
    if (buffer == NULL)
    {
        printf("malloc(): Not enough memory (%ld)\n", size);
        wait_forever();
    }

    size_t bytes = fread(buffer, 1, size, f);
    if (bytes != size)
    {
        printf("fread(%ld): %zu bytes read", size, bytes);
        wait_forever();
    }
    buffer[size - 1] = '\0';

    printf("[%s]\n", buffer);

    int res = fclose(f);
    if (res != 0)
    {
        perror("fclose");
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

    // It isn't needed to call fatInitDefault() manually. If nitroFSInit detects
    // that the ROM is running in a flashcard or from the DSi internal SD slot,
    // it will call it internally.
    bool init_ok = nitroFSInit(NULL);
    if (!init_ok)
    {
        perror("nitroFSInit()");
        wait_forever();
    }

    fflush(stdout);
    consoleSelect(&bottomScreen);

    // Change the current working directory to the base directory used for
    // testing.

    chdir("nitro:/fstest");

    char *cwd = getcwd(NULL, 0);
    printf("Current dir: %s\n", cwd);
    free(cwd);
    wait_press_button_a();

    // Print the contents of multiple directories.

    if (chdir("d1") != 0)
    {
        perror("chdir(d1)");
        wait_forever();
    }
    dir_list();

    if (chdir("d1d1") != 0)
    {
        perror("chdir(d1d1)");
        wait_forever();
    }
    dir_list();

    if (chdir("../d1d2") != 0)
    {
        perror("chdir(../d1d2)");
        wait_forever();
    }
    dir_list();

    if (chdir("../..") != 0)
    {
        perror("chdir(../..)");
        wait_forever();
    }
    dir_list();

    // Open a file, print its contents. Then, reopen it, seek some arbitrary
    // point in the middle of the file and print the rest of the contents.

    {
        const char *path = "long_file.txt";
        fcat(path);

        printf("\n\n");

        printf("cat: %s\n", path);

        FILE *f = fopen(path, "r");
        if (f == NULL)
        {
            perror("fopen");
            wait_forever();
        }

        int ret = fseek(f, 0, SEEK_END);
        if (ret != 0)
        {
            perror("fseek");
            wait_forever();
        }

        long size = ftell(f);
        if (size == -1)
        {
            perror("ftell");
            wait_forever();
        }

        printf("Size: %ld bytes\n", size);

        rewind(f);

        long remaining_size = size - (size / 2);
        ret = fseek(f, size / 2, SEEK_CUR);
        if (ret != 0)
        {
            perror("fseek");
            wait_forever();
        }

        char *buffer = malloc(remaining_size + 1);
        if (buffer == NULL)
        {
            printf("malloc(): Not enough memory (%ld)\n", remaining_size);
            wait_forever();
        }

        size_t bytes = fread(buffer, 1, remaining_size, f);
        if (bytes != remaining_size)
        {
            printf("fread(%ld): %zu bytes read", remaining_size, bytes);
            wait_forever();
        }
        buffer[remaining_size - 1] = '\0';

        printf("[%s]\n", buffer);

        int res = fclose(f);
        if (res != 0)
        {
            perror("fclose");
            wait_forever();
        }

        free(buffer);
    }
    wait_press_button_a();

    // Use stat() on a directory and a file and see the results.

    {
        printf("\x1b[2J"); // Clear console

        struct stat st;
        if (stat("d1", &st) != 0)
        {
            perror("stat(d1)");
            wait_forever();
        }
        printf("d1:\n\n  size = %lu\n  type = %s\n  date = %lld\n\n",
                st.st_size,
                (st.st_mode & S_IFDIR) ? "Dir" : "File",
                st.st_mtim.tv_sec);

        if (stat("f1.txt", &st) != 0)
        {
            perror("stat(d1)");
            wait_forever();
        }
        printf("f1.txt:\n\n  size = %lu\n  type = %s\n  date = %lld\n",
                st.st_size,
                (st.st_mode & S_IFDIR) ? "Dir" : "File",
                st.st_mtim.tv_sec);
    }
    wait_press_button_a();

    // Check access to a file that doesn't exist. Then, check access to a file
    // that exists.

    {
        if (access("d1/doesnt_exist.txt", F_OK) == 0)
        {
            printf("access() should have failed!");
            wait_forever();
        }

        if (access("d1/d1f1.txt", F_OK) != 0)
        {
            perror("access");
            wait_forever();
        }
    }
    wait_press_button_a();

    // Clear console
    printf("\x1b[2J");
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
