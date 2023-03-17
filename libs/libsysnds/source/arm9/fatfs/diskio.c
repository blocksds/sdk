/*------------------------------------------------------------------------/
/  Low level disk I/O module SKELETON for FatFs                           /
/-------------------------------------------------------------------------/
/
/ Copyright (C) 2019, ChaN, all right reserved.
/ Copyright (C) 2023, AntonioND, all right reserved.
/
/ FatFs module is an open source software. Redistribution and use of FatFs in
/ source and binary forms, with or without modification, are permitted provided
/ that the following condition is met:
/
/ 1. Redistributions of source code must retain the above copyright notice,
/    this condition and the following disclaimer.
/
/ This software is provided by the copyright holder and contributors "AS IS"
/ and any warranties related to this software are DISCLAIMED.
/ The copyright owner or contributors be NOT LIABLE for any damages caused
/ by use of this software.
/
/----------------------------------------------------------------------------*/

//-----------------------------------------------------------------------
// If a working storage control module is available, it should be
// attached to the FatFs via a glue function rather than modifying it.
// This is an example of glue functions to attach various exsisting
// storage control modules to the FatFs module with a defined API.
//-----------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <nds/arm9/cache.h>
#include <nds/arm9/dldi.h>
#include <nds/card.h>
#include <nds/memory.h>
#include <nds/system.h>

#include <fatfs.h>

#include "ff.h"     // Obtains integer types
#include "diskio.h" // Declarations of disk functions
#include "cache.h"

// Definitions of physical drive number for each drive
#define DEV_DLDI    0 // DLDI driver (flashcard)
#define DEV_SD      1 // SD slot of the DSi
#define DEV_NITRO   2 // Filesystem included in NDS ROM

// Disk I/O behaviour configuration
#define IO_CACHE_IGNORE_LARGE_READS 2

// NOTE: The clearStatus() function of DISC_INTERFACE isn't used in libfat, so
// it isn't needed here either.

static bool fs_initialized[FF_VOLUMES];
static const DISC_INTERFACE *fs_io[FF_VOLUMES];

static FILE *nitro_file;
static uint32_t nitro_fat_offset;

#if FF_MAX_SS != FF_MIN_SS
#error "This file assumes that the sector size is always the same".
#endif

//-----------------------------------------------------------------------
// Get Drive Status
//-----------------------------------------------------------------------

// pdrv: Physical drive nmuber to identify the drive
DSTATUS disk_status(BYTE pdrv)
{
    switch (pdrv)
    {
        case DEV_DLDI:
        case DEV_SD:
        case DEV_NITRO:
            return fs_initialized[pdrv] ? 0 : STA_NOINIT;

        default:
            return STA_NOINIT;
    }
}

//-----------------------------------------------------------------------
// Initialize a Drive
//-----------------------------------------------------------------------

// pdrv: Physical drive nmuber to identify the drive
DSTATUS disk_initialize(BYTE pdrv)
{
    // TODO: Should we fail if the device has been initialized, or succeed?
    if (fs_initialized[pdrv])
        return 0;

    switch (pdrv)
    {
        case DEV_DLDI:
        {
            const DISC_INTERFACE *io = dldiGetInternal();

            if (!io->startup())
                return STA_NOINIT;

            if (!io->isInserted())
                return STA_NODISK;

            fs_io[pdrv] = io;
            fs_initialized[pdrv] = true;

            return 0;
        }
        case DEV_SD:
        {
            const DISC_INTERFACE *io = get_io_dsisd();

            if (!io->startup())
                return STA_NOINIT;

            if (!io->isInserted())
                return STA_NODISK;

            fs_io[pdrv] = io;
            fs_initialized[pdrv] = true;

            return 0;
        }
        case DEV_NITRO:
        {
            if (__NDSHeader->fatSize == 0)
                return STA_NODISK;

            nitro_fat_offset = __NDSHeader->fatOffset;

            int argc = __system_argv->argc;
            char **argv = __system_argv->argv;

            // NitroFAT checks if the path of the NDS ROM has been passed in
            // argv[0]. If not, it defaults to Slot-1 card commands.
            //
            // Out of all emulators I've tried only DeSmuMe fills the argv info

            nitro_file = NULL; // Use card commands

            if (argc > 0)
            {
                if (strlen(argv[0]) > 0)
                {
                    fatInitDefault();
                    nitro_file = fopen(argv[0], "rb");
                }
            }

            fs_initialized[pdrv] = true;

            return 0;
        }
    }
    return STA_NOINIT;
}

//-----------------------------------------------------------------------
// Read Sector(s)
//-----------------------------------------------------------------------

#define NDS_CARD_BLOCK_SIZE 0x200

// Size must be smaller or equal to NDS_CARD_BLOCK_SIZE
static void cardReadBlock(void *dest, uint32_t offset, uint32_t size)
{
    const uint32_t flags =
        CARD_DELAY1(0x1FFF) | CARD_DELAY2(0x3F) | CARD_CLK_SLOW |
        CARD_nRESET | CARD_SEC_CMD | CARD_SEC_DAT | CARD_ACTIVATE |
        CARD_BLK_SIZE(1);

    cardParamCommand(CARD_CMD_DATA_READ, offset, flags, dest, size);
}

// The destination and size must be word-aligned
static void cardRead(void *dest, uint32_t offset, uint32_t size)
{
    char *curr_dest = dest;

    while (size > 0)
    {
        // The cardReadBlock() function can only read up to NDS_CARD_BLOCK_SIZE
        uint32_t curr_size = size;
        if (curr_size > NDS_CARD_BLOCK_SIZE)
            curr_size = NDS_CARD_BLOCK_SIZE;

        cardReadBlock(curr_dest, offset, curr_size);
        curr_dest += curr_size;
        offset += curr_size;
        size -= curr_size;
    }
}

// pdrv:   Physical drive nmuber to identify the drive
// buff:   Data buffer to store read data
// sector: Start sector in LBA
// count:  Number of sectors to read
DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
    if (!fs_initialized[pdrv])
        return RES_NOTRDY;

    switch (pdrv)
    {
        case DEV_DLDI:
        case DEV_SD:
        {
            const DISC_INTERFACE *io = fs_io[pdrv];
#ifdef IO_CACHE_IGNORE_LARGE_READS
            if (count >= IO_CACHE_IGNORE_LARGE_READS)
            {
                // Is the target in main RAM?
                if (((uint32_t) buff) >> 24 == 0x02 && !(((uint32_t) buff) & 0x03))
                {
                    if (!io->readSectors(sector, count, buff))
                        return RES_ERROR;

                    return RES_OK;
                }
            }
#endif
            while (count > 0)
            {
                void *cache = cache_sector_get(pdrv, sector);
                if (cache != NULL)
                {
                    memcpy(buff, cache, FF_MAX_SS);
                }
                else
                {
                    void *cache = cache_sector_add(pdrv, sector);

                    if (!io->readSectors(sector, 1, cache))
                        return RES_ERROR;

                    memcpy(buff, cache, FF_MAX_SS);
                }

                count--;
                sector++;
                buff += FF_MAX_SS;
            }

            return RES_OK;
        }
        case DEV_NITRO:
        {
            uint32_t offset = nitro_fat_offset + sector * FF_MAX_SS;
            uint32_t size = count * FF_MAX_SS;

            if (nitro_file != NULL)
            {
                // Only use the cache if NitroFAT is reading directly from the
                // cartridge with card read commands. When reading from the
                // filesystem, it is already cached in the other devices.

                if (fseek(nitro_file, offset, SEEK_SET) != 0)
                    return RES_ERROR;

                if (fread(buff, 1, size, nitro_file) != size)
                    return RES_ERROR;
            }
            else
            {
#ifdef IO_CACHE_IGNORE_LARGE_READS
                if (count >= IO_CACHE_IGNORE_LARGE_READS)
                {
                    cardRead(buff, 0, count * FF_MAX_SS);
                    return RES_OK;
                }
#endif
                while (count > 0)
                {
                    void *cache = cache_sector_get(pdrv, sector);
                    if (cache != NULL)
                    {
                        memcpy(buff, cache, FF_MAX_SS);
                    }
                    else
                    {
                        void *cache = cache_sector_add(pdrv, sector);

                        cardRead(cache, offset, FF_MAX_SS);

                        memcpy(buff, cache, FF_MAX_SS);
                    }

                    count--;
                    sector++;
                    offset += FF_MAX_SS;
                    buff += FF_MAX_SS;
                }
            }

            return RES_OK;
        }
    }

    return RES_PARERR;
}

//-----------------------------------------------------------------------
// Write Sector(s)
//-----------------------------------------------------------------------

#if FF_FS_READONLY == 0

// pdrv:   Physical drive nmuber to identify the drive
// buff:   Data to be written
// sector: Start sector in LBA
// count:  Number of sectors to write
DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
    uint8_t *align_buffer;
    if (fs_initialized[pdrv] == 0)
        return RES_NOTRDY;

    switch (pdrv)
    {
        case DEV_DLDI:
        case DEV_SD:
        {
            for (uint32_t i = 0; i < count; i++)
                cache_sector_invalidate(pdrv, sector + i);

            const DISC_INTERFACE *io = fs_io[pdrv];
            if (((uint32_t) buff) & 0x03)
            {
                // DLDI drivers expect a 4-byte aligned buffer.
                align_buffer = malloc(FF_MAX_SS);
                if (align_buffer == NULL)
                    return RES_ERROR;

                while (count > 0)
                {
                    memcpy(align_buffer, buff, FF_MAX_SS);
                    if (!io->writeSectors(sector, 1, align_buffer))
                        return RES_ERROR;

                    count--;
                    sector++;
                    buff += FF_MAX_SS;
                }

                free(align_buffer);
            }
            else
            {
                if (!io->writeSectors(sector, count, buff))
                    return RES_ERROR;
            }

            return RES_OK;
        }
        case DEV_NITRO:
        {
            // This filesystem is read-only
            return RES_WRPRT;
        }
    }

    return RES_PARERR;
}

#endif

//-----------------------------------------------------------------------
// Miscellaneous Functions
//-----------------------------------------------------------------------

// pdrv: Physical drive nmuber (0..)
// cmd:  Control code
// buff: Buffer to send/receive control data
DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    (void)buff;

    if (!fs_initialized[pdrv])
        return RES_NOTRDY;

    // Only CTRL_SYNC is needed for now:
    // - GET_SECTOR_COUNT: Used by f_mkfs and f_fdisk.
    // - GET_SECTOR_SIZE: Required only if FF_MAX_SS > FF_MIN_SS.
    // - GET_BLOCK_SIZE: Used by f_mkfs.
    // - CTRL_TRIM: Required when FF_USE_TRIM == 1.

    switch (pdrv)
    {
        case DEV_DLDI:
        case DEV_SD:
        case DEV_NITRO:
            // This command flushes the cache, but there is no cache right now
            if (cmd == CTRL_SYNC)
                return RES_OK;

            return RES_PARERR;

        default:
            return RES_PARERR;
    }
}

DWORD get_fattime(void)
{
    time_t t = time(0);
    struct tm *stm = localtime(&t);

    return (DWORD)(stm->tm_year - 80) << 25 |
           (DWORD)(stm->tm_mon + 1) << 21 |
           (DWORD)stm->tm_mday << 16 |
           (DWORD)stm->tm_hour << 11 |
           (DWORD)stm->tm_min << 5 |
           (DWORD)stm->tm_sec >> 1;
}
