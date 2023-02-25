// SPDX-License-Identifier: Zlib
//
// Copyright (c) 2023 Antonio Niño Díaz

#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <time.h>
#include <sys/_timeval.h>

// This file implements stubs for system calls. For more information about it,
// check the documentation of newlib and picolibc:
//
//     https://sourceware.org/newlib/libc.html#Syscalls
//     https://github.com/picolibc/picolibc/blob/main/doc/os.md

void __libnds_exit(int rc);

// System calls

int getpid(void)
{
    return 1;
}

int _kill(int pid, int sig)
{
    (void)pid;
    (void)sig;

    errno = EINVAL;
    return -1;
}

void __attribute__((noreturn)) _exit(int status)
{
    _kill(status, -1);

    __libnds_exit(status);

    // Hang, there is nowhere to go
    while (1);
}

clock_t times(struct tms *buf)
{
    (void)buf;

    return -1;
}

int fork(void)
{
    errno = EAGAIN;
    return -1;
}

int gettimeofday(struct timeval *tp, void *tz)
{
    // Defined in libnds
    extern time_t *punixTime;

    if (tp != NULL)
    {
        tp->tv_sec = *punixTime;
        tp->tv_usec = 0;
    }

    return 0;
}
