// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023-2026
// SPDX-FileContributor: Adrian "asie" Siekierka, 2023

#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <utime.h>

#include <nds.h>
#include <nds/arm9/dldi.h>
#include <fat.h>

__attribute__((noreturn)) void wait_forever(void)
{
    printf("\nPress START to exit to loader\n");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();
        if (keysDown() & KEY_START)
            exit(0);
    }
}

void check_date_time(const char *filename, const char *reference_mtime)
{
    struct stat st = { 0 };

    printf("stat(%s)\n", filename);

    if (stat(filename, &st) != 0)
    {
        perror("stat()");
        wait_forever();
    }

    char mtime_str[30];
    char ctime_str[30];

    ctime_r(&st.st_mtime, mtime_str);
    mtime_str[strlen(mtime_str) - 1] = '\0';
    ctime_r(&st.st_ctime, ctime_str);
    ctime_str[strlen(ctime_str) - 1] = '\0';

    printf("mtime: %s\nctime: %s\nsize: %lu B\n", mtime_str, ctime_str, st.st_size);

    if (reference_mtime != NULL)
    {
        if (strcmp(mtime_str, reference_mtime) != 0)
        {
            printf("ERROR: Date mismatch\n[%s]\n[%s]", mtime_str, reference_mtime);
            fprintf(stderr, "[TEST] Test failed\n");
            wait_forever();
        }
    }
}

int main(int argc, char **argv)
{
    consoleDemoInit();
    consoleDebugInit(DebugDevice_NOCASH);

    printf("DLDI name: %s\n", io_dldi_data->friendlyName);
    printf("DSi mode: %d\n", isDSiMode());

    if (argc < 1)
    {
        printf("ERROR: argv[0] not available");
        wait_forever();
    }

    bool init_ok = fatInitDefault();
    if (!init_ok)
    {
        perror("fatInitDefault()");
        wait_forever();
    }

    // ----------------

    check_date_time(argv[0], NULL);

    // ----------------

    struct utimbuf times;

    printf("\nutime(946717840)\n");
    times.modtime = 946717840;
    if (utime(argv[0], &times) != 0)
    {
        perror("utime()");
        wait_forever();
    }
    check_date_time(argv[0], "Sat Jan  1 09:10:40 2000");

    // ----------------

    printf("\nutime(1100991600)\n");
    times.modtime = 1100991600;
    if (utime(argv[0], &times) != 0)
    {
        perror("utime()");
        wait_forever();
    }
    check_date_time(argv[0], "Sat Nov 20 23:00:00 2004");

    // ----------------

    fprintf(stderr, "[TEST] Test passed\n");

    wait_forever();

    return 0;
}
