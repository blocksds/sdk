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

#undef errno
extern int errno;

char *__env[1] = { 0 };
char **environ = __env;

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

// Defined in libnds
void __libnds_exit(int rc);

void _exit(int status)
{
    _kill(status, -1);

    __libnds_exit(status);

    // Hang, there is nowhere to go
    while (1);
}

// The file descriptors for stdin, stdout, and stderr are 0, 1, and 2.
#define FD_STDIN    0
#define FD_STDOUT   1
#define FD_STDERR   2

// Defined in libnds
typedef ssize_t (* fn_stdin_read)(char *, size_t);
extern fn_stdin_read libnds_stdin_read;

int _read(int file, char *ptr, int len)
{
    if (file == FD_STDIN)
    {
        if (libnds_stdin_read)
            return libnds_stdin_read(ptr, len);
    }

    return 0;
}

// Defined in libnds
typedef ssize_t (* fn_write_ptr)(const char *, size_t);
extern fn_write_ptr libnds_stdout_write, libnds_stderr_write;

int _write(int file, char *ptr, int len)
{
    if (file == FD_STDIN)
        return 0;

    if (file == FD_STDOUT)
    {
        if (libnds_stderr_write)
            len = libnds_stdout_write(ptr, len);
    }

    if (file == FD_STDERR)
    {
        if (libnds_stderr_write)
            len = libnds_stderr_write(ptr, len);
    }

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

int _gettimeofday(struct timeval *tp, void *tz)
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

void *fake_heap_end = NULL;
void *fake_heap_start = NULL;

void *_sbrk(int incr)
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
