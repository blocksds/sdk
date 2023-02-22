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

// The file descriptors for stdin, stdout, and stderr are 0, 1, and 2.
#define FD_STDIN    0
#define FD_STDOUT   1
#define FD_STDERR   2

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

int read(int file, char *ptr, int len)
{
    if (file == FD_STDIN)
        return 0;

    return 0;
}

int write(int file, char *ptr, int len)
{
    if ((file >= FD_STDIN) && (file <= FD_STDERR))
        return 0;

    return 0;
}

int close(int file)
{
    (void)file;

    return -1;
}

int fstat(int file, struct stat *st)
{
    (void)file;

    st->st_mode = S_IFCHR;
    return 0;
}

int isatty(int file)
{
    (void)file;

    return 1;
}

int lseek(int file, int ptr, int dir)
{
    (void)file;
    (void)ptr;
    (void)dir;

    return 0;
}

int open(char *path, int flags, ...)
{
    (void)path;
    (void)flags;

    return -1;
}

int unlink(const char *name)
{
    (void)name;

    errno = ENOENT;
    return -1;
}

clock_t times(struct tms *buf)
{
    (void)buf;

    return -1;
}

int _stat(char *file, struct stat *st)
{
    (void)file;

    st->st_mode = S_IFCHR;
    return 0;
}

int link(char *old, char *new)
{
    (void)old;
    (void)new;

    errno = EMLINK;
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
