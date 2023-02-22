// SPDX-License-Identifier: Zlib
//
// Copyright (c) 2023 Antonio Niño Díaz

#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <time.h>

// This file implements stubs for system calls. For more information about it,
// check the documentation of newlib:
//
//     https://sourceware.org/newlib/libc.html#Syscalls

int _getpid(void)
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

void _exit(int status)
{
    _kill(status, -1);

    // Hang, there is nowhere to go
    while (1);
}

__attribute__((weak)) int _read(int file, char *ptr, int len)
{
    (void)file;
    (void)ptr;

    return len;
}

__attribute__((weak)) int _write(int file, char *ptr, int len)
{
    (void)file;
    (void)ptr;

    return len;
}

int _close(int file)
{
    (void)file;

    return -1;
}


int _fstat(int file, struct stat *st)
{
    (void)file;

    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file)
{
    (void)file;

    return 1;
}

int _lseek(int file, int ptr, int dir)
{
    (void)file;
    (void)ptr;
    (void)dir;

    return 0;
}

int _open(char *path, int flags, ...)
{
    (void)path;
    (void)flags;

    return -1;
}

int _wait(int *status)
{
    (void)status;

    errno = ECHILD;
    return -1;
}

int _unlink(char *name)
{
    (void)name;

    errno = ENOENT;
    return -1;
}

int _times(struct tms *buf)
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

int _link(char *old, char *new)
{
    (void)old;
    (void)new;

    errno = EMLINK;
    return -1;
}

int _fork(void)
{
    errno = EAGAIN;
    return -1;
}

int _execve(char *name, char **argv, char **env)
{
    (void)name;
    (void)argv;
    (void)env;

    errno = ENOMEM;
    return -1;
}

void *_sbrk(int incr)
{
    errno = ENOMEM;
    return (void *)-1;
}
