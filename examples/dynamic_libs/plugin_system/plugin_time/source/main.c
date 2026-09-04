// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025-2026

#include <time.h>

#define SYM_PUBLIC  __attribute__((visibility ("default")))
#define SYM_LOCAL   __attribute__((visibility ("hidden")))

time_t time(time_t *second);

typedef int (*ptr_printf)(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
typedef time_t (*ptr_time)(time_t *);

ptr_printf my_printf;
ptr_time my_time;

static time_t start_time;

SYM_PUBLIC void PLUGIN_SetInterfacePrintf(ptr_printf p)
{
    my_printf = p;
}

SYM_PUBLIC void PLUGIN_SetInterfaceTime(ptr_time t)
{
    my_time = t;

    start_time = my_time(NULL);
}

SYM_PUBLIC int PLUGIN_Run(int argument)
{
    my_printf("[P][T] time(): %llu\n", my_time(NULL) - start_time);

    return 0;
}
