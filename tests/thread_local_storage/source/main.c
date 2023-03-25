// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <stdio.h>

#include <nds.h>

__thread int var_tdata = 1000;
__thread int var_tbss = 0;

void print_thread_info(char *name)
{
    cothread_info_t *p = (void *)cothread_get_current();

    printf("\n");
    printf("TLS:        %X [%s]\n", (uintptr_t)p->tls, name);
    printf("&var_tdata: %X\n", (uintptr_t)&var_tdata);
    printf("&var_tbss:  %X\n", (uintptr_t)&var_tbss);
}

int entrypoint_thread(void *arg)
{
    print_thread_info(arg);
    return 0;
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    printf("Press START to exit to loader\n");
    printf("\n");

    extern char __tdata_start[];
    extern char __tdata_size[];
    extern char __tbss_start[];
    extern char __tbss_size[];
    extern char __tls_start[];
    extern char __tls_size[];

    // Addresses of the reference tdata and tbss sections. Note that the size of
    // tbss includes the size of errno.
    printf("__tdata:    %X (%u)\n", (uintptr_t)__tdata_start, (uintptr_t)__tdata_size);
    printf("__tbss:     %X (%u)\n", (uintptr_t)__tbss_start, (uintptr_t)__tbss_size);
    // Address of the space reserved for the main() thread
    printf("__tls:      %X (%u)\n", (uintptr_t)__tls_start, (uintptr_t)__tls_size);

    // The value of TLS for main should match __tls_start
    print_thread_info("main");

    // Thread local storage sections of secondary threads should be after this
    printf("\n");
    printf("sbrk(0):    %X\n", (uintptr_t)sbrk(0));

    cothread_create(entrypoint_thread, "thread1", 0, COTHREAD_DETACHED);
    cothread_create(entrypoint_thread, "thread2", 0, COTHREAD_DETACHED);
    cothread_create(entrypoint_thread, "thread3", 0, COTHREAD_DETACHED);

    while (1)
    {
        cothread_yield_irq(IRQ_VBLANK);

        scanKeys();

        if (keysDown() & KEY_START)
            break;
    }

    return 0;
}
