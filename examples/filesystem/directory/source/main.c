// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <dirent.h>

#include <errno.h>
#include <stdio.h>

#include <fatfs.h>
#include <nds.h>

#include <nds/arm9/dldi.h>

void dir_list(void)
{
    DIR *dirp = opendir(".");
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

    printf("\nNum entries: %d\n", num_entries);
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    printf("\x1b[2J"); // Clear console

    printf("DLDI name:\n%s\n\n", io_dldi_data->friendlyName);
    printf("DSi mode: %d\n\n", isDSiMode());

    fflush(stdout);

    bool init_ok = fatInitDefault();
    if (!init_ok)
    {
        perror("fatInitDefault()");
    }
    else
    {
        char *cwd = getcwd(NULL, 0);
        printf("Current dir: %s\n\n", cwd);
        free(cwd);

        dir_list();
    }

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
