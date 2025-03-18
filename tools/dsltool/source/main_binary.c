// SPDX-License-Identifier: Zlib
//
// Copyright (C) 2025 Antonio Niño Díaz

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "elf.h"

static Elf32_Ehdr *hdr = NULL;

static int symtab_index = -1;
static size_t symtab_size = 0;

static int strtab_index = -1;

int main_binary_load(const char *path)
{
    hdr = elf_load(path);
    if (hdr == NULL)
    {
        printf("Failed to open: %s\n", path);
        return -1;
    }

    for (unsigned int i = 0; i < hdr->e_shnum; i++)
    {
        const Elf32_Shdr *shdr = elf_section(hdr, i);

        // Exclude sections with no name
        const char *name = elf_get_string_shstrtab(hdr, shdr->sh_name);
        if (name == NULL)
            continue;

        // Exclude empty sections
        size_t size = shdr->sh_size;
        if (size == 0)
            continue;

        if (strcmp(name, ".strtab") == 0)
        {
            strtab_index = i;
        }
        else if (strcmp(name, ".symtab") == 0)
        {
            symtab_index = i;
            symtab_size = size;
        }
    }

    if ((symtab_index == -1) || (strtab_index == -1))
    {
        printf("Can't find strab or symtab\n");
        free(hdr);
        return -1;
    }

    printf("Found %zu symbols\n", symtab_size / sizeof(Elf32_Sym));

    return 0;
}

bool main_binary_is_loaded(void)
{
    return hdr != NULL;
}

uint32_t main_binary_get_symbol_value(const char *name)
{
    if (hdr == NULL)
        return UINT32_MAX;

    const Elf32_Sym *sym = elf_section_data(hdr, symtab_index);
    size_t sym_num = symtab_size / sizeof(Elf32_Sym);

    for (size_t s = 0; s < sym_num; s++, sym++)
    {
        uint8_t type = ELF_ST_TYPE(sym->st_info);

        const char *sym_name;

        // Only save addresses of functions and objects, not sections
        if ((type != STT_FUNC) && (type != STT_OBJECT))
            continue;

        sym_name = elf_get_string_strtab(hdr, strtab_index, sym->st_name);

        if (strcmp(sym_name, name) != 0)
            continue;

        return sym->st_value;
    }

    return UINT32_MAX;
}

void main_binary_free(void)
{
    free(hdr);
}
