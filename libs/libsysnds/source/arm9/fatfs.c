// SPDX-License-Identifier: Zlib
//
// Copyright (c) 2023 Antonio Niño Díaz

#include <errno.h>
#include <stdbool.h>

#include <nds/memory.h>
#include <nds/system.h>

#include "fatfs/cache.h"
#include "fatfs/ff.h"

#define DEFAULT_CACHED_SECTORS (40) // Each sector is 512 bytes

// Devices: "fat:/", "sd:/", "nitro:/"
static FATFS fs_info[FF_VOLUMES] = { 0 };

static bool fat_initialized = false;
static bool nitrofat_initialized = false;

bool nitrofat_reader_is_arm9 = false;

int fatfs_error_to_posix(FRESULT error)
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
    if (error > FR_INVALID_PARAMETER) // error is unsigned
        return ENOMSG;

    return codes[error];
}

bool fatInitDefault(void)
{
    static bool has_been_called = false;

    if (has_been_called == true)
        return fat_initialized;

    has_been_called = true;

    // In DSi mode, require that there is a SD card in the SD card slot. In DS
    // mode, require that there is a flashcard, that this ROM has been DLDI
    // patched, and that DLDI can be initialized.

    bool require_fat = false, require_sd = false;

    if (isDSiMode())
        require_sd = true;
    else
        require_fat = true;

    const char *fat_drive = "fat:/";
    const char *sd_drive = "sd:/";

    int ret = cache_init(DEFAULT_CACHED_SECTORS);
    if (ret != 0)
    {
        errno = ENOMEM;
        return false;
    }

    // Try to initialize DLDI on DS and DSi
    FRESULT result = f_mount(&fs_info[0], fat_drive, 1);
    if ((result != FR_OK) && require_fat)
    {
        errno = fatfs_error_to_posix(result);
        return false;
    }

    if (isDSiMode())
    {
        // On DSi, try to initialize the internal SD slot
        result = f_mount(&fs_info[1], sd_drive, 1);
        if ((result != FR_OK) && require_sd)
        {
            errno = fatfs_error_to_posix(result);
            return false;
        }

        // Default to using the internal SD slot
        result = f_chdrive(sd_drive);
        if (result != FR_OK)
        {
            errno = fatfs_error_to_posix(result);
            return false;
        }
    }
    else
    {
        // On DS, use the DLDI driver
        result = f_chdrive(fat_drive);
        if (result != FR_OK)
        {
            errno = fatfs_error_to_posix(result);
            return false;
        }
    }

    fat_initialized = true;

    return true;
}

bool nitroFSInit(char **basepath)
{
    static bool has_been_called = false;

    if (has_been_called == true)
        return nitrofat_initialized;

    has_been_called = true;

    (void)basepath;

    const char *nitro_drive = "nitro:/";

    sysSetBusOwners(BUS_OWNER_ARM9, BUS_OWNER_ARM9);

    FRESULT result = f_mount(&fs_info[2], nitro_drive, 1);
    if (result != FR_OK)
    {
        errno = fatfs_error_to_posix(result);
        return false;
    }

    nitrofat_initialized = true;

    return true;
}

void nitroFATSetReaderCPU(bool use_arm9)
{
    nitrofat_reader_is_arm9 = use_arm9;
}
