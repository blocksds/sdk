// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

#include <stdint.h>

#define SYM_PUBLIC  __attribute__((visibility ("default")))
#define SYM_LOCAL   __attribute__((visibility ("hidden")))

SYM_PUBLIC void cypher_rot13(void *buffer, uint32_t size)
{
    char *b = buffer;

    for (int i = 0; i < size; i++)
    {
        char c = *b;

        if ((c >= 'N' && c <= 'Z') || (c >= 'n' && c <= 'z'))
            c -= 13;
        else if ((c >= 'A' && c <= 'M') || (c >= 'a' && c <= 'm'))
            c += 13;

        *b = c;

        b++;
    }
}
