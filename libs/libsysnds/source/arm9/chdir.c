// SPDX-License-Identifier: Zlib
//
// Copyright (c) 2023 Antonio Niño Díaz

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "fatfs/ff.h"
#include "fatfs_internal.h"

int chdir(const char *path)
{
    int ret = -1;

    // Copy string to be able to modify it freely
    char *full = strdup(path);
    if (full == NULL)
    {
        errno = ENOMEM;
        return -1;
    }

    // Split full path into drive plus dir
    char *drive = NULL;
    char *dir = NULL;

    char *divide = strstr(full, ":/");
    if (divide != NULL)
    {
        divide[0] = '\0';
        drive = full;
        dir = divide + 1;
    }
    else
    {
        dir = full;
    }

    // Remove all trailing '/' from the dir path
    size_t len = strlen(dir);
    while (len > 0)
    {
        if (full[len - 1] != '/')
            break;

        full[len - 1] = 0;
        len--;
    }

    if (strlen(dir) == 0)
        dir = "/";

    FRESULT result = FR_OK;
    if (drive != NULL)
    {
        result = f_chdrive(drive);
        if (result != FR_OK)
        {
            errno = fatfs_error_to_posix(result);
            goto cleanup;
        }
    }

    result = f_chdir(dir);
    if (result != FR_OK)
    {
        errno = fatfs_error_to_posix(result);
        goto cleanup;
    }

    // Success
    ret = 0;
cleanup:
    free(full);
    return ret;
}

char *getcwd(char *buf, size_t size)
{
    if (buf == NULL)
    {
        // Extension to POSIX.1-2001 standard. If buf is NULL, the buffer is
        // allocated by getcwd() using malloc(). The size is the one provided by
        // the caller. If the size is zero, it is allocated as big as necessary.
        // As an extension to the , glibc's getcwd() allocates the buffer. The
        // caller must free this buffer after using it.

        int optimize_mem = 0;

        if (size == 0)
        {
            size = PATH_MAX + 1;
            optimize_mem = 1;
        }

        buf = calloc(size, 1);
        if (buf == NULL)
        {
            errno = ENOMEM;
            return NULL;
        }

        FRESULT result = f_getcwd(buf, size - 1);
        if (result != FR_OK)
        {
            free(buf);
            errno = ENAMETOOLONG;
            return NULL;
        }
        buf[size - 1] = '\0';

        if (optimize_mem)
        {
            // Allocate new string that uses just the required space and free
            // the temporary one.
            char *ret = strdup(buf);
            free(buf);
            return ret;
        }
        else
        {
            return buf;
        }
    }
    else
    {
        FRESULT result = f_getcwd(buf, size - 1);
        if (result != FR_OK)
        {
            free(buf);
            errno = ENAMETOOLONG;
            return NULL;
        }
        buf[size - 1] = '\0';

        return buf;
    }
}
