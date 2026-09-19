// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023
// SPDX-FileContributor: Adrian "asie" Siekierka, 2023

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <utime.h>

void print_date_time(char *filename)
{
    struct stat st;

    if (stat(filename, &st) != 0)
    {
        perror("stat()");
        exit(0);
    }

    printf("time: %s\n", ctime(&st.st_mtime));
}

int main(int argc, char **argv)
{
    struct utimbuf times;

    if (argc < 1)
    {
        printf("ERROR: argv[0] not available");
        exit(0);
    }

    printf("\nstat()\n");
    print_date_time(argv[0]);

    printf("\nutime(946717840)\n");
    times.modtime = 946717840;
    if (utime(argv[0], &times) != 0)
    {
        perror("utime()");
        exit(0);
    }
    print_date_time(argv[0]);

    printf("\nutime(1100991600)\n");
    times.modtime = 1100991600;
    if (utime(argv[0], &times) != 0)
    {
        perror("utime()");
        exit(0);
    }
    print_date_time(argv[0]);

    return 0;
}
