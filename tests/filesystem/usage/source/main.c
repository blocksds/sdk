// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023-2026

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
    printf("Press A to continue\n");

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
    while (1)
        swiWaitForVBlank();
}

void dir_list(void)
{
    consoleClear();

    printf("path: %s\n", getcwd(NULL, 0));
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

    printf("\nNum entries: %d\n\n", num_entries);

    wait_press_button_a();

    consoleClear();
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

    consoleClear();

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

    // Change the current working directory to the base directory used for
    // testing.

    printf("chdir to test directory...\n\n");

    chdir("fstest");

    char *cwd = getcwd(NULL, 0);
    printf("Current dir: %s\n\n", cwd);
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

    if (chdir("../../d2") != 0)
    {
        perror("chdir(../../d2)");
        wait_forever();
    }
    dir_list();

    // Print contents of a file. Then, write more contents to it, and close it.

    printf("Original contents:\n\n");

    fcat("d2_remove.txt");

    {
        printf("\nAppending data...\n\n");

        FILE *f = fopen("d2_remove.txt", "a");
        if (f == NULL)
        {
            perror("fopen");
            wait_forever();
        }

        char buffer[] = "--Test string--";
        size_t size = sizeof(buffer);
        size_t bytes = fwrite(buffer, 1, size, f);
        if (bytes != size)
        {
            printf("fread(%zu): %zu bytes read", size, bytes);
            wait_forever();
        }

        int res = fclose(f);
        if (res != 0)
        {
            perror("fclose");
            wait_forever();
        }
    }

    // Reopen the same file and print the contents again to verify that new data
    // was written to it correctly.

    printf("\nNew contents:\n\n");

    fcat("d2_remove.txt");

    // Delete the file and list the contents of the directory again (which
    // should be empty now).

    printf("\nDeleting...\n\n");

    unlink("d2_remove.txt");

    wait_press_button_a();

    dir_list();

    // Try to open the file after closing it, which should fail.

    printf("Try to open deleted file...\n\n");

    {
        FILE *f = fopen("d2_remove.txt", "r");
        if (f != NULL)
        {
            printf("d2_remove.txt wasn't removed");
            wait_forever();
        }
    }

    // Try opening files in all possible modes. Only 'r' and 'r+' should fail

    printf("Opening files in all modes...\n\n");

    {
        FILE *f;

        f = fopen("r.txt", "r");
        if (f != NULL)
        {
            printf("r.txt shouldn't be created");
            wait_forever();
        }

        f = fopen("r+.txt", "r+");
        if (f != NULL)
        {
            printf("r+.txt shouldn't be created");
            wait_forever();
        }

        f = fopen("w.txt", "w");
        if (f == NULL)
        {
            printf("failed to open w.txt");
            wait_forever();
        }
        if (fclose(f) != 0)
        {
            printf("failed to close w.txt");
            wait_forever();
        }

        f = fopen("w+.txt", "w+");
        if (f == NULL)
        {
            printf("failed to open w+.txt");
            wait_forever();
        }
        if (fclose(f) != 0)
        {
            printf("failed to close w+.txt");
            wait_forever();
        }

        f = fopen("a.txt", "a");
        if (f == NULL)
        {
            printf("failed to open a.txt");
            wait_forever();
        }
        if (fclose(f) != 0)
        {
            printf("failed to close a.txt");
            wait_forever();
        }

        f = fopen("a+.txt", "a+");
        if (f == NULL)
        {
            printf("failed to open a+.txt");
            wait_forever();
        }
        if (fclose(f) != 0)
        {
            printf("failed to close a+.txt");
            wait_forever();
        }

        wait_press_button_a();

        dir_list();
    }

    // Open a file, print its contents. Then, reopen it, seek some arbitrary
    // point in the middle of the file and print the rest of the contents.

    if (chdir("..") != 0)
    {
        perror("chdir(..)");
        wait_forever();
    }
    dir_list();

    {
        printf("Original contents:\n\n");

        const char *path = "long_file.txt";
        fcat(path);

        printf("\nOpening again...\n\n");

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

        printf("\nSeeking middle of the file...\n\n");

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

        printf("[%s]\n\n", buffer);

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
        consoleClear();

        printf("Getting file information...\n\n");

        struct stat st;
        if (stat("d1", &st) != 0)
        {
            perror("stat(d1)");
            wait_forever();
        }
        // Don't print the date, it will change every time the test runs
        printf("d1:\n\n  size = %lu\n  type = %s\n\n",
                st.st_size,
                (st.st_mode & S_IFDIR) ? "Dir" : "File");

        if (stat("f1.txt", &st) != 0)
        {
            perror("stat(d1)");
            wait_forever();
        }
        printf("f1.txt:\n\n  size = %lu\n  type = %s\n\n",
                st.st_size,
                (st.st_mode & S_IFDIR) ? "Dir" : "File");
    }
    wait_press_button_a();

    // Try to create a folder with a name that is used by a file. Then, create
    // a new folder and move a file to it.

    consoleClear();

    {
        printf("Creating folders...\n\n");

        if (mkdir("f1.txt", S_IRWXU | S_IRWXG | S_IRWXO) == 0)
        {
            printf("Folder created with same name as file!");
            wait_forever();
        }

        if (mkdir("new_dir", S_IRWXU | S_IRWXG | S_IRWXO) != 0)
        {
            perror("mkdir(new_dir)");
            wait_forever();
        }

        printf("Folder created\n\n");

        if (rename("long_file.txt", "new_dir/renamed.txt") != 0)
        {
            perror("rename");
            wait_forever();
        }

        printf("File renamed\n\n");
    }

    wait_press_button_a();

    // Check access to a file that doesn't exist. Then, check access to a file
    // that exists, and truncate it. Check that the size is the expected one.

    {
        consoleClear();

        printf("Testing access to files...\n\n");

        const char *path = "new_dir/renamed.txt";

        if (access("new_dir/doesnt_exist.txt", F_OK) == 0)
        {
            printf("access() should have failed!");
            wait_forever();
        }

        if (access(path, F_OK) != 0)
        {
            perror("access");
            wait_forever();
        }

        printf("Testing truncate()...\n\n");

        if (truncate(path, 10) != 0)
        {
            perror("truncate(10)");
            wait_forever();
        }

        struct stat st;
        if (stat(path, &st) != 0)
        {
            perror("stat(renamed.txt)");
            wait_forever();
        }
        printf("stat:\n\n  size = %lu\n  type = %s\n\n",
                st.st_size, (st.st_mode & S_IFDIR) ? "Dir" : "File");

        if (st.st_size != 10)
        {
            printf("Invalid size: %lu != 10\n", st.st_size);
            wait_forever();
        }

        if (truncate(path, 1024) != 0)
        {
            perror("truncate(1024)");
            wait_forever();
        }

        if (stat(path, &st) != 0)
        {
            perror("stat(renamed.txt)");
            wait_forever();
        }
        printf("stat:\n\n  size = %lu\n  type = %s\n\n",
                st.st_size, (st.st_mode & S_IFDIR) ? "Dir" : "File");

        if (st.st_size != 1024)
        {
            printf("Invalid size: %lu != 1024\n", st.st_size);
            wait_forever();
        }
    }
    wait_press_button_a();

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
