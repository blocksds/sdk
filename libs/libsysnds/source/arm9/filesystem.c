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
#include <sys/unistd.h>
#include <time.h>

#include "fatfs/ff.h"
#include "fatfs_internal.h"

// This file implements stubs for system calls. For more information about it,
// check the documentation of newlib and picolibc:
//
//     https://sourceware.org/newlib/libc.html#Syscalls
//     https://github.com/picolibc/picolibc/blob/main/doc/os.md

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

ssize_t read(int fd, void *ptr, size_t len)
{
    // This isn't handled here
    if (fd == STDIN_FILENO)
        return -1;

    FIL *fp = (FIL *)fd;
    UINT bytes_read = 0;

    FRESULT result = f_read(fp, ptr, len, &bytes_read);

    if (result == FR_OK)
        return bytes_read;

    errno = fatfs_error_to_posix(result);
    return -1;
}

ssize_t write(int fd, const void *ptr, size_t len)
{
    // This isn't handled here
    if ((fd >= STDIN_FILENO) && (fd <= STDERR_FILENO))
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

int stat(const char *path, struct stat *st)
{
    FILINFO fno = { 0 };
    FRESULT result = f_stat(path, &fno);

    if (result != FR_OK)
    {
        errno = fatfs_error_to_posix(result);
        return -1;
    }

    st->st_size = fno.fsize;

#if FF_MAX_SS != FF_MIN_SS
#error "Set the block size to the right value"
#endif
    st->st_blksize = FF_MAX_SS;
    st->st_blocks = (fno.fsize + FF_MAX_SS - 1) / FF_MAX_SS;

    st->st_mode = (fno.fattrib & AM_DIR) ?
                   S_IFDIR : // Directory
                   S_IFREG;  // Regular file

    struct tm timeinfo = { 0 };
    timeinfo.tm_year   = ((fno.fdate >> 9) + 1980) - 1900;
    timeinfo.tm_mon    = ((fno.fdate >> 5) & 15) - 1;
    timeinfo.tm_mday   = fno.fdate & 31;
    timeinfo.tm_hour   = fno.ftime >> 11;
    timeinfo.tm_min    = (fno.ftime >> 5) & 63;
    timeinfo.tm_sec    = (fno.ftime & 31) * 2;

    time_t time = mktime(&timeinfo);

    // If there is any problem determining the modification timestam, just leave
    // it empty.
    if (time == (time_t)-1)
        time = 0;

    st->st_atim.tv_sec = time; // Time of last access
    st->st_mtim.tv_sec = time; // Time of last modification
    st->st_ctim.tv_sec = time; // Time of last status change

    return 0;
}

int isatty(int fd)
{
    (void)fd;

    return 0;
}

int link(const char *old, const char *new)
{
    (void)old;
    (void)new;

    errno = EMLINK;
    return -1;
}

int rename(const char *old, const char *new)
{
    FRESULT result = f_rename(old, new);

    if (result == FR_OK)
        return 0;

    errno = fatfs_error_to_posix(result);
    return -1;
}

static int ftruncate_internal(int fd, off_t length)
{
    // This function assumes that the new lenght is different from the current
    // one, so it doesn't have any shortcuts in case they are the same. The
    // callers must implement them.

    FIL *fp = (FIL *)fd;

    FSIZE_t fsize = f_size(fp);

    // If the new size is bigger, it's not enough to use f_lseek to set the
    // pointer to the new size, or to use f_expand. Both of them increase the
    // size of the file, but the contents are undefined. According to the
    // documentation of truncate() the new contents need to be zeroed. The only
    // possible way to do this with FatFs is to simply append zeroes to the end
    // of the file.
    //
    // If the new file is smaller, it is enough to call f_lseek to set the
    // pointer to the new size, and then call f_truncate.

    if ((size_t)length > (size_t)fsize)
    {
        // Expand the file to a bigger size

        FRESULT result = f_lseek(fp, fsize);
        if (result != FR_OK)
        {
            errno = fatfs_error_to_posix(result);
            return -1;
        }

        FSIZE_t size_diff = length - fsize;

        char zeroes[128] = { 0 };

        while (size_diff > 128)
        {
            if (write(fd, zeroes, 128) == -1)
                return -1;

            size_diff -= 128;
        }

        if (size_diff > 0)
        {
            if (write(fd, zeroes, size_diff) == -1)
                return -1;
        }
    }
    else // if (length < fsize)
    {
        // Truncate the file to a smaller size

        FRESULT result = f_lseek(fp, length);
        if (result != FR_OK)
        {
            errno = fatfs_error_to_posix(result);
            return -1;
        }

        result = f_truncate(fp);
        if (result != FR_OK)
        {
            errno = fatfs_error_to_posix(result);
            return -1;
        }
    }

    return 0;
}

int ftruncate(int fd, off_t length)
{
    FIL *fp = (FIL *)fd;

    if ((size_t)length == (size_t)f_size(fp))
        return 0; // There is nothing to do

    // Preserve the current pointer
    FSIZE_t prev_offset = f_tell(fp);

    int ftruncate_ret = ftruncate_internal(fd, length);
    int ftruncate_errno = errno;

    // Try to return pointer to its previous position even if the truncate
    // function has failed (but return the previous errno value).
    FSIZE_t new_offset = lseek(fd, prev_offset, SEEK_SET);

    if (ftruncate_ret != 0)
    {
        errno = ftruncate_errno;
        return -1;
    }

    if (new_offset != prev_offset)
        return -1;

    return 0;
}

int truncate(const char *path, off_t length)
{
    int fd = open(path, O_RDWR);
    if (fd == -1)
        return -1;

    FIL *fp = (FIL *)fd;
    if ((size_t)length != (size_t)f_size(fp))
    {
        int ret = ftruncate_internal(fd, length);
        if (ret != 0)
            return -1;
    }

    int ret = close(fd);
    if (ret != 0)
        return -1;

    return 0;
}

int mkdir(const char *path, mode_t mode)
{
    (void)mode; // There are no permissions in FAT filesystems

    FRESULT result = f_mkdir(path);
    if (result != FR_OK)
    {
        errno = fatfs_error_to_posix(result);
        return -1;
    }

    return 0;
}

int chmod(const char *path, mode_t mode)
{
    // The only attributes that FAT supports are "Read only", "Archive",
    // "System" and "Hidden". This doesn't match very well with UNIX
    // permissions, so this funciton simply does nothing.

    (void)path;
    (void)mode;

    errno = ENOSYS;
    return -1;
}

int chown(const char *path, uid_t owner, gid_t group)
{
    // FAT doesn't support file and group owners.

    (void)path;
    (void)owner;
    (void)group;

    errno = ENOSYS;
    return -1;
}

int access(const char *path, int amode)
{
    int open_flags = 0;

    if (amode == F_OK)
    {
         open_flags = O_RDONLY;
    }
    else
    {
        // Ignore R_OK and X_OK. Always test for read access, and test for write
        // access if requested.
        if (amode & W_OK)
            open_flags = O_RDWR;
        else
            open_flags = O_RDONLY;
    }

    int fd = open(path, open_flags);
    if (fd == -1)
        return -1;

    int ret = close(fd);
    if (ret == -1)
        return -1;

    return 0;
}
