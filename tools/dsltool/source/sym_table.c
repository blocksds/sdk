// SPDX-License-Identifier: Zlib
//
// Copyright (C) 2025 Antonio Niño Díaz

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dsl.h"
#include "log.h"
#include "main_binary.h"

typedef struct {
    const char *name;
    uint32_t value;
    unsigned int initial_index;
    bool public;
    bool used;
    bool unknown;
} elf_symbol_info;

elf_symbol_info *elf_symbols;
size_t elf_symbols_num = 0;

void sym_add_to_table(const char *name, uint32_t value, bool is_public, bool is_unknown)
{
    size_t new_size = sizeof(elf_symbol_info) * (elf_symbols_num + 1);
    elf_symbols = realloc(elf_symbols, new_size);

    elf_symbols[elf_symbols_num].name = name;
    elf_symbols[elf_symbols_num].value = value;
    elf_symbols[elf_symbols_num].used = is_public;
    elf_symbols[elf_symbols_num].public = is_public;
    elf_symbols[elf_symbols_num].unknown = is_unknown;
    elf_symbols[elf_symbols_num].initial_index = elf_symbols_num;

    elf_symbols_num++;
}

int sym_set_as_used(unsigned int index)
{
    if (index >= elf_symbols_num)
        return -1;

    elf_symbols[index].used = true;
    return 0;
}

bool sym_is_unknown(unsigned int index)
{
    if (index >= elf_symbols_num)
        return false;

    return elf_symbols[index].unknown;
}

const char *sym_get_name(unsigned int index)
{
    if (index >= elf_symbols_num)
        return false;

    return elf_symbols[index].name;
}

int sym_get_sym_index_by_old_index(unsigned int index)
{
    elf_symbol_info *sym = elf_symbols;

    for (size_t i = 0; i < elf_symbols_num; i++, sym++)
    {
        if (sym->initial_index == index)
            return i;
    }

    return -1;
}

static int sym_compare_entries(const void *p1, const void *p2)
{
    const char *name1 = ((elf_symbol_info *)p1)->name;
    const char *name2 = ((elf_symbol_info *)p2)->name;

    return strcmp(name1, name2);
}

void sym_clear_unused(void)
{
    size_t i = 0;
    while (1)
    {
        if (i >= elf_symbols_num)
            break;

        if (elf_symbols[i].used)
        {
            i++;
            continue;
        }

        for (size_t j = i; j < elf_symbols_num - 1; j++)
            elf_symbols[j] = elf_symbols[j + 1];

        elf_symbols_num--;
    }
}

void sym_sort_table(void)
{
    qsort(elf_symbols, elf_symbols_num, sizeof(elf_symbol_info), sym_compare_entries);
}

void sym_clear_table(void)
{
    free(elf_symbols);
    elf_symbols = NULL;
    elf_symbols_num = 0;
}

void sym_print_table(void)
{
    elf_symbol_info *sym = elf_symbols;

    VERBOSE("Symbol table (%zu entries)\n", elf_symbols_num);

    for (size_t i = 0; i < elf_symbols_num; i++, sym++)
    {
        VERBOSE("%zu: \"%s\" = %u%s%s\n", i, sym->name, sym->value,
                sym->public ? " [Public]" : "", sym->unknown ? " [Unknown]" : "");
    }
}

int sym_table_save_to_file(FILE *f)
{
    dsl_symbol_table header = {
        .num_symbols = elf_symbols_num,
        .unused = {0},
    };

    if (fwrite(&header, sizeof(dsl_symbol_table), 1, f) != 1)
    {
        ERROR("Failed to write DSL header\n");
        return -1;
    }

    // We're going to start writing strings right after the symbol array.
    uint32_t offset = sizeof(dsl_symbol_table)
                    + sizeof(dsl_symbol) * elf_symbols_num;

    for (size_t i = 0; i < elf_symbols_num; i++)
    {
        dsl_symbol sym = {
            .name_str_offset = offset,
            .value = elf_symbols[i].value,
            .attributes = 0,
        };

        if (elf_symbols[i].public)
            sym.attributes |= DSL_SYMBOL_PUBLIC;

        // Check if this symbol is unknown. If so, look for it in the main
        // binary and edit the symbol to define the address.
        if (elf_symbols[i].unknown)
        {
            const char *sym_name = sym_get_name(i);
            VERBOSE("Unknown symbol [%s]\n", sym_name);

            // Look for the symbol in the main binary
            if (!main_binary_is_loaded())
            {
                ERROR("No main binary provided. Can't resolve address for [%s]\n",
                      sym_name);
                return -1;
            }

            VERBOSE("Searching main binary...\n");

            uint32_t sym_addr = main_binary_get_symbol_value(sym_name);
            if (sym_addr == UINT32_MAX)
            {
                ERROR("Symbol not found: [%s]\n", sym_name);
                return -1;
            }

            VERBOSE("Symbol found: 0x%08X\n", sym_addr);

            sym.value = sym_addr;
            sym.attributes |= DSL_SYMBOL_MAIN_BINARY;
        }

        // Allocate space for this name
        offset += strlen(elf_symbols[i].name) + 1;

        if (fwrite(&sym, sizeof(dsl_symbol), 1, f) != 1)
        {
            ERROR("Failed to write symbol %zu: [%s]\n", i, elf_symbols[i].name);
            return -1;
        }
    }

    for (size_t i = 0; i < elf_symbols_num; i++)
    {
        const char *name = elf_symbols[i].name;
        size_t len = strlen(name) + 1; // Include NUL terminator

        if (fwrite(name, 1, len, f) != len)
        {
            ERROR("Failed to write symbol name %zu: [%s]\n", i, elf_symbols[i].name);
            return -1;
        }
    }

    return 0;
}
