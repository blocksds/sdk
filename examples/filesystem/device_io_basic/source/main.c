// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2026

// This example shows how to add a new custom filesystem to libnds, which is
// mounted at "lfs:/". This drive is a ramdisk that uses littlefs:
//
//     https://github.com/littlefs-project/littlefs (commit adad0fbbcf53)
//
// The example also uses NitroFS to demonstrate that you can easily switch
// between both filesystems with chdir("nitro:/") and chdir("lfs:/").
//
// In this example the ramdisk is initialized at boot every time. It's possible
// to, for example, save it to a file in the filesystem and load it later. This
// example doesn't try to do that because it would add complexity and it isn't
// needed to show how to use the device API of libnds.

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>

#include <filesystem.h>
#include <nds.h>
#include <nds/arm9/dldi.h>

#include "littlefs_device.h"

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

void dir_list(const char *path)
{
    printf("Listing files at %s\n\n", path);

    DIR *dirp = opendir(path);
    if (dirp == NULL)
    {
        perror("opendir");
        return;
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

        // Only print up to 10 entries
        if (num_entries < 10)
        {
            int index = telldir(dirp);
            printf("%d - %s%s\n", index, cur->d_name,
                (cur->d_type == DT_DIR) ? "/" : " ");
        }
        else if (num_entries == 10)
        {
            printf("[...]\n");
        }
    }

    closedir(dirp);
}

void create_folder(const char *path)
{
    printf("Creating folder %s\n", path);
    mkdir(path, 0777);
}

void create_file(const char *path, const char *data)
{
    printf("Creating %s\n", path);

    FILE *f = fopen(path, "wb");
    if (f == NULL)
    {
        printf("fopen(%s, wb) failed: %d", path, errno);
        wait_forever();
    }

    size_t size = strlen(data);
    if (fwrite(data, 1, size, f) != size)
    {
        printf("fwrite(%s) failed: %d", path, errno);
        wait_forever();
    }

    if (fclose(f) != 0)
    {
        printf("fclose(%s) failed: %d", path, errno);
        wait_forever();
    }
}

void print_file(const char *path)
{
    printf("Reading %s\n", path);

    FILE *f = fopen(path, "rb");
    if (f == NULL)
    {
        printf("fopen(%s, rb) failed: %d", path, errno);
        wait_forever();
    }

    // Read as much as the buffer can hold
    char buf[200];
    int end = fread(buf, 1, sizeof(buf) - 1, f);
    buf[end] = '\0';

    if (fclose(f) != 0)
    {
        printf("fclose(%s) failed: %d", path, errno);
        wait_forever();
    }

    printf("[%s]\n", buf);
}

void delete_file(const char *path)
{
    printf("Deleting %s\n", path);
    if (unlink(path) != 0)
    {
        printf("unlink(%s) failed: %d", path, errno);
        wait_forever();
    }
}

void print_file_stat(const char *path)
{
    printf("Stat %s\n", path);

    struct stat st = { 0 };
    if (stat(path, &st) != 0)
    {
        printf("stat(%s) failed: %d", path, errno);
        wait_forever();
    }

    printf("\n  size = %lu\n  type = %s\n",
           st.st_size,
           (st.st_mode & S_IFDIR) ? "Dir" : "File");
}

void set_working_directory(const char *path)
{
    printf("Setting cwd: %s\n", path);
    if (chdir(path) != 0)
    {
        printf("chdir(%s) failed: %d", path, errno);
        wait_forever();
    }

    char buf[30];
    if (getcwd(buf, sizeof(buf)) == NULL)
    {
        printf("getcwd(%s) failed: %d", path, errno);
        wait_forever();
    }

    printf("New cwd: %s\n", buf);
}

int main(int argc, char **argv)
{
    PrintConsole topScreen;
    PrintConsole bottomScreen;

    defaultExceptionHandler();

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

    printf("Initializing NitroFS...\n");

    if (!nitroFSInit(NULL))
    {
        perror("nitroFSInit");
        wait_forever();
    }

    printf("NitroFS ready!\n");
    printf("\n");

    littlefs_init();

    printf("\n");

    set_working_directory("lfs:/");

    consoleSelect(&bottomScreen);

    consoleClear();
    create_folder("/dir");
    create_file("/dir/test.txt", "File inside of a folder");
    create_file("/test.txt", "File in the root directory");
    create_file("/test2.txt", "Some text here");
    printf("\n");
    printf("Press A to continue");

    wait_press_button_a();

    consoleClear();
    print_file("/dir/test.txt");
    printf("\n");
    print_file("/test.txt");
    printf("\n");
    dir_list("/");
    printf("\n");
    dir_list("/dir");
    printf("\n");
    printf("Press A to continue");

    wait_press_button_a();

    consoleClear();
    print_file_stat("/dir/test.txt");
    printf("\n");
    print_file_stat("/dir");
    printf("\n");
    print_file_stat("/test.txt");
    printf("\n");
    print_file_stat("/test2.txt");
    printf("\n");
    printf("Press A to continue");

    wait_press_button_a();

    consoleClear();
    set_working_directory("nitro:/");
    printf("\n");
    dir_list("/fstest");
    printf("\n");
    set_working_directory("lfs:/");

    wait_press_button_a();

    consoleClear();
    delete_file("/test.txt");
    printf("\n");
    delete_file("/dir/test.txt");
    printf("\n");
    dir_list("/");
    printf("\n");
    dir_list("/dir");
    printf("\n");
    printf("Press A to continue");

    wait_press_button_a();

    consoleClear();
    delete_file("/dir");
    printf("\n");
    delete_file("/test2.txt");
    printf("\n");
    dir_list("/");
    printf("\n");
    printf("Press A to continue");

    wait_press_button_a();

    consoleClear();
    printf("End of example!\n");
    printf("\n");
    printf("Press START to exit to loader\n");

    while (1)
    {
        swiWaitForVBlank();
        scanKeys();
        if (keysDown() & KEY_START)
            break;
    }

    littlefs_end();

    return 0;
}
