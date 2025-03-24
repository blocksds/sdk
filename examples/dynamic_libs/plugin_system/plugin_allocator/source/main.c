// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

#include <stddef.h>

#define SYM_PUBLIC  __attribute__((visibility ("default")))
#define SYM_LOCAL   __attribute__((visibility ("hidden")))

typedef int (*ptr_printf)(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
typedef void *(*ptr_malloc)(size_t);
typedef void (*ptr_free)(void *);

ptr_printf my_printf;
ptr_malloc my_malloc;
ptr_free my_free;

SYM_PUBLIC void PLUGIN_SetInterfacePrintf(ptr_printf p)
{
    my_printf = p;
}

SYM_PUBLIC void PLUGIN_SetInterfaceMalloc(ptr_malloc m, ptr_free f)
{
    my_malloc = m;
    my_free = f;
}

SYM_PUBLIC int PLUGIN_Run(int argument)
{
    if (argument < 1)
    {
        my_printf("[P][A] Invalid size: %d\n", argument);
        return -1;
    }

    void *p = my_malloc(argument);

    my_printf("[P][A] malloc(): %p\n", p);

    my_free(p);

    return 0;
}
