// SPDX-License-Identifier: Zlib
//
// Copyright (c) 2023 Antonio Niño Díaz

#include <stddef.h>
#include <stdint.h>
#include <errno.h>

void *fake_heap_end = NULL;
void *fake_heap_start = NULL;

void *sbrk(int incr)
{
    // Symbol defined by the linker
    extern char __end__[];
    const uintptr_t end = (uintptr_t)__end__;

    // Trick to get the current stack pointer
    register uintptr_t stack_ptr asm("sp");

    // Next address to be used. It is updated after every call to sbrk()
    static uintptr_t heap_start = 0;

    if (heap_start == 0)
    {
        if (fake_heap_start == NULL)
            heap_start = end;
        else
            heap_start = (uintptr_t)fake_heap_start;
    }

    // Current limit
    uintptr_t heap_end;

    if (fake_heap_end == NULL)
        heap_end = stack_ptr;
    else
        heap_end = (uintptr_t)fake_heap_end;

    // Try to allocate
    if (heap_start + incr > heap_end)
    {
        errno = ENOMEM;
        return (void *)-1;
    }

    uintptr_t prev_heap_start = heap_start;

    heap_start += incr;

    return (void *)prev_heap_start;
}
