// SPDX-License-Identifier: Zlib
//
// Copyright (c) 2023 Antonio Niño Díaz

#ifndef FATFS_CACHE_H__
#define FATFS_CACHE_H__

#include <stdint.h>
#include <stddef.h>

int cache_init(uint32_t num_sectors);
void *cache_sector_get(uint8_t pdrv, uint32_t sector);
void *cache_sector_add(uint8_t pdrv, uint32_t sector);
void cache_sector_invalidate(uint8_t pdrv, uint32_t sector);

#endif // FATFS_CACHE_H__
