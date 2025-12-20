// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <dirent.h>

#include <errno.h>
#include <stdio.h>

#include <fat.h>
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

        int index = telldir(dirp);
        printf("%d - %s%s\n", index, cur->d_name,
               (cur->d_type == DT_DIR) ? "/" : " ");

        num_entries++;

        // Don't print too many entries
        if (num_entries == 6)
            break;
    }

    closedir(dirp);

    printf("\nNum entries: %d\n", num_entries);
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    printf("DLDI name:\n%s\n\n", io_dldi_data->friendlyName);
    printf("DSi mode: %d\n\n", isDSiMode());

    // Check that the DLDI name isn't the default one
    const char *nodldi = "Default (No interface)";
    if (strcmp(nodldi, io_dldi_data->friendlyName) == 0)
    {
        printf("ROM not patched with DLDI.\n");
        goto end;
    }

    printf("A: Autodetect\n");
    printf("L: Force ARM7\n");
    printf("R: Force ARM9\n");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint32_t keys_down = keysDown();
        if (keys_down & KEY_A)
        {
            dldiSetMode(DLDI_MODE_AUTODETECT);
            break;
        }

        if (keys_down & KEY_L)
        {
            dldiSetMode(DLDI_MODE_ARM7);
            break;
        }

        if (keys_down & KEY_R)
        {
            dldiSetMode(DLDI_MODE_ARM9);
            break;
        }
    }

    bool init_ok = fatInitDefault();
    if (!init_ok)
    {
        perror("fatInitDefault()");
    }
    else
    {
        chdir("fat:/");

        char *cwd = getcwd(NULL, 0);
        printf("Current dir: %s\n\n", cwd);
        free(cwd);

        dir_list();
    }

    printf("\n");
    printf("DLDI_MODE: %d\n", dldiGetMode());

end:
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
