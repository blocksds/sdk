// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <dirent.h>
#include <stdio.h>
#include <time.h>

#include <fatfs.h>
#include <nds.h>

#include <nds/arm9/dldi.h>

#include "md5/md5.h"

// -------------------------------------------------

uint32_t my_rand(void)
{
    static uint32_t seed = 0;

    seed = seed * 0xFDB97531 + 0x2468ACE;

    return seed;
}

// -------------------------------------------------

#define MAX_FILES 500

char *file_path[MAX_FILES];
int file_path_num;

void add_file_name(const char *path)
{
    if (file_path_num >= MAX_FILES)
        return;

    file_path[file_path_num++] = strdup(path);
}

char *get_random_name(void)
{
    int index = my_rand() % file_path_num;
    return file_path[index];
}

// -------------------------------------------------

void scan_dir(const char *path)
{
    DIR *dirp = opendir(path);
    if (dirp == NULL)
    {
        perror("opendir");
        return;
    }

    while (1)
    {
        struct dirent *cur = readdir(dirp);
        if (cur == NULL)
            break;

        if (strlen(cur->d_name) == 0)
            break;

        char full_name[512];
        snprintf(full_name, sizeof(full_name), "%s/%s", path, cur->d_name);

        if (cur->d_type == DT_DIR)
            scan_dir(full_name);
        else
            add_file_name(full_name);
    }

    closedir(dirp);
}

// -------------------------------------------------

// This reads the file and returns 1 if the file name matches the MD5 hash.
int verify_md5_file(char *path)
{
    FILE *f = fopen(path, "r");
    if (f == NULL)
    {
        perror("fopen(random.bin)");
        return 0;
    }

    MD5Context ctx;
    md5Init(&ctx);

    uint8_t input_buffer[1024];

    while (1)
    {
        size_t input_size = fread(input_buffer, 1, 1024, f);
        if (input_size <= 0)
            break;

        md5Update(&ctx, (uint8_t *)input_buffer, input_size);
    }

    md5Finalize(&ctx);

    fclose(f);

    // Generate string with the md5 hash

    uint8_t digest[16];
    memcpy(digest, ctx.digest, 16);

    char digest_str[32 + 1];
    for (int i = 0; i < 16; i++)
        sprintf(digest_str + (i * 2), "%02x", digest[i]);

    //printf("Calculated\n%s", digest_str);

    // Get the file name from the full path

    size_t len = strlen(path);
    char *base_name = path;

    for (size_t l = len; l > 0; l--)
    {
        if (path[l] == '/')
        {
            base_name = path + l + 1;
            break;
        }
    }

    //printf("Base name\n%s", base_name);

    // Compare the two md5 values

    int compare = strcmp(base_name, digest_str);

    //printf("Result: %d", compare);

    return (compare == 0);
}

// -------------------------------------------------

int main(int argc, char **argv)
{
    consoleDemoInit();

    printf("\x1b[2J"); // Clear console

    printf("DLDI name:\n");
    printf("%s\n", io_dldi_data->friendlyName);
    printf("\n");
    printf("DSi mode: %d\n\n", isDSiMode());

    fflush(stdout);

    bool init_ok = nitroFSInit(NULL);
    if (!init_ok)
    {
        perror("nitroFSInit()");
        goto exit;
    }

    chdir("nitro:/");

    char *cwd = getcwd(NULL, 0);
    printf("Current dir: %s\n\n", cwd);
    free(cwd);

    time_t start_time = time(NULL);

    // Scan directories and fill the list of files

    scan_dir(".");

    // Calculate MD5 hash of many random files

    for (int i = 0; i < 200; i++)
    {
        printf(".");

        char *name = get_random_name();
        if (verify_md5_file(name) == 0)
        {
            printf("Hash doesn't match!\n%s\n\n", name);
            goto exit;
        }
    }

    time_t total_time = time(NULL) - start_time;

    printf("\n\nTotal time: %llu seconds\n", total_time);

exit:
    printf("\n");
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
