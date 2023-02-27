// SPDX-License-Identifier: Zlib
//
// Copyright (c) 2023 Antonio Niño Díaz

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/_timeval.h>
#include <time.h>

#include <ff.h>

// This file implements stubs for system calls. For more information about it,
// check the documentation of newlib and picolibc:
//
//     https://sourceware.org/newlib/libc.html#Syscalls
//     https://github.com/picolibc/picolibc/blob/main/doc/os.md

// The file descriptors for stdin, stdout, and stderr are 0, 1, and 2.
#define FD_STDIN    0
#define FD_STDOUT   1
#define FD_STDERR   2

static int fatfs_error_to_posix(FRESULT error)
{
    // The following errno codes have been picked so that they make some sort of
    // sense, but also so that they can be told apart.

    const FRESULT codes[] = {
        // Succeeded
        [FR_OK] = 0,
        // A hard error occurred in the low level disk I/O layer
        [FR_DISK_ERR] = EIO,
        // Assertion failed
        [FR_INT_ERR] = EFAULT,
        // The physical drive cannot work
        [FR_NOT_READY] = ECANCELED,
         // Could not find the file
        [FR_NO_FILE] = ENOENT,
         // Could not find the path
        [FR_NO_PATH] = ENOENT,
        // The path name format is invalid
        [FR_INVALID_NAME] = EINVAL,
         // Access denied due to prohibited access or directory full
        [FR_DENIED] = EACCES,
        // Access denied due to prohibited access
        [FR_EXIST] = EEXIST,
        // The file/directory object is invalid
        [FR_INVALID_OBJECT] = EBADF,
        // The physical drive is write protected
        [FR_WRITE_PROTECTED] = EROFS,
        // The logical drive number is invalid
        [FR_INVALID_DRIVE] = EINVAL,
        // The volume has no work area
        [FR_NOT_ENABLED] = ENOMEM,
        // There is no valid FAT volume
        [FR_NO_FILESYSTEM] = ENODEV,
        // The f_mkfs() aborted due to any problem
        [FR_MKFS_ABORTED] = ENXIO,
        // Could not get a grant to access the volume within defined period
        [FR_TIMEOUT] = ETIME,
        // The operation is rejected according to the file sharing policy
        [FR_LOCKED] = EPERM,
        // LFN working buffer could not be allocated
        [FR_NOT_ENOUGH_CORE] = ENOMEM,
        // Number of open files > FF_FS_LOCK
        [FR_TOO_MANY_OPEN_FILES] = ENOSR,
        // Given parameter is invalid
        [FR_INVALID_PARAMETER] = EINVAL,
    };

    // If this ever happens, there has been a serious error in FatFs
    if ((error > FR_INVALID_PARAMETER) || (error < 0))
        return ENOMSG;

    return codes[error];
}

int open(const char *path, int flags, ...)
{
    // POSIX | FatFs
    // ------+----------------------------------------
    // "r"   | FA_READ
    // "r+"  | FA_READ | FA_WRITE
    // "w"   | FA_CREATE_ALWAYS | FA_WRITE
    // "w+"  | FA_CREATE_ALWAYS | FA_WRITE | FA_READ
    // "wx"  | FA_CREATE_NEW | FA_WRITE
    // "w+x" | FA_CREATE_NEW | FA_WRITE | FA_READ
    // "a"   | FA_OPEN_APPEND | FA_WRITE
    // "a+"  | FA_OPEN_APPEND | FA_WRITE | FA_READ

    // POSIX | open()
    // ------+----------------------------------------
    // "r"   | O_RDONLY
    // "r+"  | O_RDWR
    // "w"   | O_WRONLY | O_CREAT | O_TRUNC
    // "w+"  | O_RDWR   | O_CREAT | O_TRUNC
    // "wx"  | O_WRONLY | O_CREAT | O_TRUNC | O_EXCL
    // "w+x" | O_RDWR   | O_CREAT | O_TRUNC | O_EXCL
    // "a"   | O_WRONLY | O_CREAT | O_APPEND
    // "a+"  | O_RDWR   | O_CREAT | O_APPEND

    // O_BINARY and O_TEXT are ignored.

    BYTE mode = 0;

    int can_write = 0;

    switch (flags & (O_RDONLY | O_WRONLY | O_RDWR))
    {
        case O_RDONLY:
            mode = FA_READ;
            break;
        case O_WRONLY:
            can_write = 1;
            mode = FA_WRITE;
            break;
        case O_RDWR:
            can_write = 1;
            mode = FA_READ | FA_WRITE;
            break;
        default: // Default to just reading
            errno = EINVAL;
            return -1;
    }

    if (can_write)
    {
        if (flags & O_CREAT)
        {
            if (flags & O_APPEND)
            {
                mode |= FA_OPEN_APPEND; // a | a+
            }
            else if (flags & O_TRUNC)
            {
                // O_EXCL isn't used by the fopen provided by picolibc.
                if (flags & O_EXCL)
                    mode |= FA_CREATE_NEW; // wx | w+x
                else
                    mode |= FA_CREATE_ALWAYS; // w | w+
            }
            else
            {
                // O_APPEND or O_TRUNC must be set if O_CREAT is set
                errno = EINVAL;
                return -1;
            }
        }
        else
        {
            mode |= FA_OPEN_EXISTING; // r+
        }
    }
    else
    {
        mode |= FA_OPEN_EXISTING; // r
    }

    FIL *fp = calloc(sizeof(FIL), 1);
    if (fp == NULL)
    {
        errno = ENOMEM;
        return -1;
    }

    FRESULT result = f_open(fp, path, mode);

    if (result == FR_OK)
        return (int)fp;

    errno = fatfs_error_to_posix(result);
    return -1;
}

int read(int fd, char *ptr, int len)
{
    // This isn't handled here
    if (fd == FD_STDIN)
        return -1;

    FIL *fp = (FIL *)fd;
    UINT bytes_read = 0;

    FRESULT result = f_read(fp, ptr, len, &bytes_read);

    if (result == FR_OK)
        return bytes_read;

    errno = fatfs_error_to_posix(result);
    return -1;
}

int write(int fd, char *ptr, int len)
{
    // This isn't handled here
    if ((fd >= FD_STDIN) && (fd <= FD_STDERR))
        return -1;

    FIL *fp = (FIL *)fd;
    UINT bytes_written = 0;

    FRESULT result = f_write(fp, ptr, len, &bytes_written);

    if (result == FR_OK)
        return bytes_written;

    errno = fatfs_error_to_posix(result);
    return -1;
}

int close(int fd)
{
    FIL *fp = (FIL *)fd;

    FRESULT result = f_close(fp);

    free(fp);

    if (result == FR_OK)
        return 0;

    errno = fatfs_error_to_posix(result);
    return -1;
}

off_t lseek(int fd, off_t offset, int whence)
{
    FIL *fp = (FIL *)fd;

    if (whence == SEEK_END)
    {
        // The file offset is set to the size of the file plus offset bytes
        whence = SEEK_SET;
        offset += f_size(fp);
    }
    else if (whence == SEEK_CUR)
    {
        // The file offset is set to its current location plus offset bytes
        whence = SEEK_SET;
        offset += f_tell(fp);
    }
    else if (whence == SEEK_SET)
    {
        // The file offset is set to offset bytes.
    }
    else
    {
        errno = EINVAL;
        return (off_t)-1;
    }

    FRESULT result = f_lseek(fp, offset);

    if (result == FR_OK)
        return offset;

    errno = fatfs_error_to_posix(result);
    return -1;
}

_off64_t lseek64(int fd, _off64_t offset, int whence)
{
    return (_off64_t)lseek(fd, (off_t)offset, whence);
}

int unlink(const char *name)
{
    FRESULT result = f_unlink(name);

    if (result == FR_OK)
        return 0;

    errno = fatfs_error_to_posix(result);
    return -1;
}

int fstat(int file, struct stat *st)
{
    (void)file;
    st->st_mode = S_IFCHR;
    return 0;
}

int isatty(int fd)
{
    return 0;
}

int link(char *old, char *new)
{
    (void)old;
    (void)new;

    errno = EMLINK;
    return -1;
}
