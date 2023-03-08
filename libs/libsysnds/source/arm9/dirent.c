// SPDX-License-Identifier: Zlib
//
// Copyright (c) 2023 Antonio Niño Díaz

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// "dirent.h" defines DIR, but "ff.h" defines a different non-standard one.
// Functions in this file need to use their standard prototypes, so it is needed
// to somehow rename the DIR of "ff.h". It's better to keep the original header
// unmodified so that updating it is easier, so this is a hack to rename it just
// in this compilation unit.
#define DIR DIRff
#include "fatfs/ff.h"
#include "fatfs_internal.h"
#undef DIR

#include <dirent.h>

#define INDEX_NO_ENTRY  -1

static DIR *alloc_dir(void)
{
    DIRff *dp = calloc(sizeof(DIRff), 1);
    if (dp == NULL)
    {
        errno = ENOMEM;
        return NULL;
    }

    DIR *dirp = calloc(sizeof(DIR), 1);
    if (dirp == NULL)
    {
        free(dp);
        errno = ENOMEM;
        return NULL;
    }

    dirp->dp = dp;
    return dirp;
}

static void free_dir(DIR *dirp)
{
    free(dirp->dp);
    free(dirp);
}

DIR *opendir(const char *name)
{
    DIR *dirp = alloc_dir();
    DIRff *dp = dirp->dp;

    dirp->index = INDEX_NO_ENTRY;

    FRESULT result = f_opendir(dp, name);
    if (result == FR_OK)
        return dirp;

    free_dir(dirp);
    errno = fatfs_error_to_posix(result);
    return NULL;
}

int closedir(DIR *dirp)
{
    if (dirp == NULL)
    {
        errno = EBADF;
        return -1;
    }

    DIRff *dp = dirp->dp;

    FRESULT result = f_closedir(dp);

    free_dir(dirp);

    if (result == FR_OK)
        return 0;

    errno = fatfs_error_to_posix(result);
    return -1;
}

struct dirent *readdir(DIR *dirp)
{
    DIRff *dp = dirp->dp;
    struct dirent *ent = &(dirp->dirent);

    memset(ent, 0, sizeof(struct dirent));

    if (dirp == NULL)
    {
        errno = EBADF;
        return NULL;
    }

    FILINFO fno = { 0 };
    FRESULT result = f_readdir(dp, &fno);
    if (result != FR_OK)
    {
        errno = fatfs_error_to_posix(result);
        return NULL;
    }

    if (strlen(fno.fname) == 0)
    {
        // End of directory reached
        dirp->index = INDEX_NO_ENTRY;
        return NULL;
    }

    dirp->index++;
    ent->d_off = dirp->index;

    strncpy(ent->d_name, fno.fname, sizeof(ent->d_name));
    fno.fname[sizeof(ent->d_name) - 1] = '\0';

    if (fno.fattrib & AM_DIR)
        ent->d_type = DT_DIR; // Directory
    else
        ent->d_type = DT_REG; // Regular file

    ent->d_reclen = sizeof(struct dirent);

    return ent;
}

void rewinddir(DIR *dirp)
{
    if (dirp == NULL)
        return;

    DIRff *dp = dirp->dp;
    (void)f_rewinddir(dp); // Ignore returned value
}

void seekdir(DIR *dirp, long loc)
{
    if (dirp == NULL)
        return;

    if (dirp->index == INDEX_NO_ENTRY) // If we're at the end or the beginning
        rewinddir(dirp);
    else if (loc < dirp->index) // If we have already passed this entry
        rewinddir(dirp);

    while (1)
    {
        // Check if the entry has been found
        if (dirp->index == loc)
            break;

        struct dirent *entry = readdir(dirp);
        if (entry == NULL)
        {
            rewinddir(dirp);
            break;
        }

        // Check if we reached the end of the directory without finding it
        if (dirp->index == INDEX_NO_ENTRY)
        {
            rewinddir(dirp);
            break;
        }
    }
}

long telldir(DIR *dirp)
{
    if (dirp == NULL)
    {
        errno = EBADF;
        return -1;
    }

    return dirp->index;
}
