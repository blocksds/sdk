// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023
// SPDX-FileContributor: Adrian "asie" Siekierka, 2023

#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <utime.h>

#include <nds.h>
#include <nds/arm9/dldi.h>
#include <fat.h>

void wait_press_button_a(void)
{
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
    while(1)
        swiWaitForVBlank();
}

void print_date_time(char *filename)
{
    struct stat st;

    if (stat(filename, &st) != 0)
    {
        perror("stat()");
        wait_forever();
    }

    printf("time: %s\n", ctime(&st.st_mtime));
}

int main(int argc, char **argv)
{
    struct utimbuf times;
    consoleDemoInit();

    printf("DLDI name: %s\n", io_dldi_data->friendlyName);
    printf("DSi mode: %d\n", isDSiMode());

    bool init_ok = fatInitDefault();
    if (!init_ok)
    {
        perror("fatInitDefault()");
        wait_forever();
    }

    printf("\nstat()\n");
    print_date_time(argv[0]);

    printf("\nutime(946717840)\n");
    times.modtime = 946717840;
    if (utime(argv[0], &times) != 0)
    {
        perror("utime()");
        wait_forever();
    }
    print_date_time(argv[0]);

    printf("\nutime(1100991600)\n");
    times.modtime = 1100991600;
    if (utime(argv[0], &times) != 0)
    {
        perror("utime()");
        wait_forever();
    }
    print_date_time(argv[0]);

    printf("\nPress START to exit to loader\n");

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
