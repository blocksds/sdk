// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <stdio.h>

#include <nds.h>

#define STACK_SIZE (1 * 1024)

extern char __tdata_start[];
extern char __tdata_size[];
extern char __tbss_start[];
extern char __tbss_size[];
extern char __tls_start[];
extern char __tls_size[];

static uintptr_t sbrk_base;
static uintptr_t next_thread_min_base;

static bool test_passed = true;

__thread int var_tdata = 1000;
__thread int var_tbss = 0;

void print_thread_info(char *name)
{
    cothread_info_t *p = (void *)cothread_get_current();

    printf("\n");
    printf("TLS:        %X [%s]\n", (uintptr_t)p->tls, name);
    printf("&var_tdata: %X (%d)\n", (uintptr_t)&var_tdata, var_tdata);
    printf("&var_tbss:  %X (%d)\n", (uintptr_t)&var_tbss, var_tbss);

    if (next_thread_min_base > (uintptr_t)p->tls)
        test_passed = false;

    // Make sure that this thread has been created after the TLS section and
    // stack memory of the previous thread.
    next_thread_min_base = (uintptr_t)p->tls + (uintptr_t)__tls_size + STACK_SIZE;

    if (var_tdata != 1000)
        test_passed = false;

    if (var_tbss != 0)
        test_passed = false;
}

int entrypoint_thread(void *arg)
{
    print_thread_info(arg);
    return 0;
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    consoleDebugInit(DebugDevice_NOCASH);

    printf("Press START to exit to loader\n");
    printf("\n");

    // Addresses of the reference tdata and tbss sections. Note that the size of
    // tbss includes the size of errno.
    printf("__tdata:    %X (%u)\n", (uintptr_t)__tdata_start, (uintptr_t)__tdata_size);
    printf("__tbss:     %X (%u)\n", (uintptr_t)__tbss_start, (uintptr_t)__tbss_size);
    // Address of the space reserved for the main() thread
    printf("__tls:      %X (%u)\n", (uintptr_t)__tls_start, (uintptr_t)__tls_size);

    if (((uintptr_t)__tdata_size + (uintptr_t)__tbss_size) != (uintptr_t)__tls_size)
        test_passed = false;

    // The value of TLS for main should match __tls_start
    print_thread_info("main");

    // Thread local storage sections of secondary threads should be after this
    printf("\n");
    sbrk_base = (uintptr_t)sbrk(0);
    printf("sbrk(0):    %X\n", sbrk_base);

    if (sbrk_base <= (uintptr_t)__tls_start)
        test_passed = false;

    next_thread_min_base = sbrk_base;

    cothread_create(entrypoint_thread, "thread1", STACK_SIZE, COTHREAD_DETACHED);
    cothread_create(entrypoint_thread, "thread2", STACK_SIZE, COTHREAD_DETACHED);
    cothread_create(entrypoint_thread, "thread3", STACK_SIZE, COTHREAD_DETACHED);

    if (test_passed)
        fprintf(stderr, "[TEST] Test passed\n");
    else
        fprintf(stderr, "[TEST] Test failed\n");

    while (1)
    {
        cothread_yield_irq(IRQ_VBLANK);

        scanKeys();

        if (keysDown() & KEY_START)
            break;
    }

    return 0;
}
