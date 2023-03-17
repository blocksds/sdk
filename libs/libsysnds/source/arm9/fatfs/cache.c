// SPDX-License-Identifier: Zlib
//
// Copyright (c) 2023 Antonio Niño Díaz

#include <stdlib.h>
#include <string.h>

#include "ff.h"

typedef struct {
    uint8_t  valid;
    uint8_t  pdrv;
    LBA_t    sector;
    uint32_t usage_count;
} cache_entry_t;

#if FF_MAX_SS != FF_MIN_SS
#error "This code expects a fixed sector size"
#endif

static cache_entry_t *cache_entries;
static uint8_t *cache_mem;
static uint32_t cache_num_sectors;

int cache_init(uint32_t num_sectors)
{
    cache_entries = calloc(num_sectors, sizeof(cache_entry_t));
    if (cache_entries == NULL)
        return -1;

    cache_mem = malloc(num_sectors * FF_MAX_SS);
    if (cache_mem == NULL)
    {
        free(cache_entries);
        return -1;
    }

    cache_num_sectors = num_sectors;

    return 0;
}

void *cache_sector_get(uint8_t pdrv, uint32_t sector)
{
    for (uint32_t i = 0; i < cache_num_sectors; i++)
    {
        cache_entry_t *entry = &(cache_entries[i]);

        if (entry->valid == 0)
            continue;

        if ((entry->pdrv != pdrv) || (entry->sector != sector))
            continue;

        if (entry->usage_count < cache_num_sectors)
            entry->usage_count++;

        return cache_mem + (i * FF_MAX_SS);
    }

    return NULL;
}

void *cache_sector_add(uint8_t pdrv, uint32_t sector)
{
    int entry_found = 0;
    static uint32_t selected_entry = 0;

    for (uint32_t i = 0; i < cache_num_sectors; i++)
    {
        if (cache_entries[selected_entry].valid == 0)
        {
            entry_found = 1;
            break;
        }

        selected_entry++;
        if (selected_entry >= cache_num_sectors)
            selected_entry = 0;
    }

    // Cache full, evict one entry
    if (entry_found == 0)
    {
        uint32_t min_usage_count = cache_entries[selected_entry].usage_count;

        if (min_usage_count > 1)
        {
            cache_entries[selected_entry].usage_count--;
            selected_entry++;
            if (selected_entry >= cache_num_sectors)
                selected_entry = 0;

            uint32_t min_entry = selected_entry;

            for (uint32_t i = 1; i < cache_num_sectors; i++)
            {
                cache_entry_t *entry = &(cache_entries[selected_entry]);

                if (entry->usage_count < min_usage_count)
                {
                    min_usage_count = entry->usage_count;
                    min_entry = selected_entry;
                }

                entry->usage_count--;
                if (min_usage_count <= 1)
                    break;

                selected_entry++;
                if (selected_entry >= cache_num_sectors)
                    selected_entry = 0;
            }

            selected_entry = min_entry;
        }
    }

    cache_entry_t *entry = &(cache_entries[selected_entry]);

    entry->pdrv = pdrv;
    entry->valid = 1;
    entry->sector = sector;
    entry->usage_count = 1;

    return cache_mem + (selected_entry * FF_MAX_SS);
}

void cache_sector_invalidate(uint8_t pdrv, uint32_t sector)
{
    for (uint32_t i = 0; i < cache_num_sectors; i++)
    {
        cache_entry_t *entry = &(cache_entries[i]);

        if (entry->valid == 0)
            continue;

        if ((entry->pdrv != pdrv) || (entry->sector != sector))
            continue;

        entry->valid = 0;
        return;
    }
}
