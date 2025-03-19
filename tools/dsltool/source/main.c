// SPDX-License-Identifier: Zlib
//
// Copyright (C) 2025 Antonio Niño Díaz

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "elf.h"
#include "dsl.h"
#include "main_binary.h"
#include "sym_table.h"

// Useful commands to analyze ELF files:
//
// 1. readelf -a path/to/elf.elf
//
// 2. WONDERFUL_TOOLCHAIN=/opt/wonderful
//    ARM_NONE_EABI_PATH=$(WONDERFUL_TOOLCHAIN)/toolchain/gcc-arm-none-eabi/bin/
//    $(ARM_NONE_EABI_PATH)/arm-none-eabi-objdump -h -C -Ssr path/to/elf.elf

void usage(void)
{
    printf("Usage: dsltool -i input.elf -o output.dsl [-m main_binary.elf]\n");
}

typedef struct {
    uint32_t address;
    uint32_t size;
    uint32_t type;
    void *data;
} elf_section_info;

#define MAX_SECTIONS 40

int main(int argc, char *argv[])
{
    printf("dsltool " PACKAGE_VERSION "\n");
    printf("=============\n");
    printf("\n");

    const char *in_file = NULL;
    const char *out_file = NULL;
    const char *main_binary_file = NULL;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-i") == 0)
        {
            i++;
            if (i < argc)
                in_file = argv[i];
        }
        else if (strcmp(argv[i], "-o") == 0)
        {
            i++;
            if (i < argc)
                out_file = argv[i];
        }
        else if (strcmp(argv[i], "-m") == 0)
        {
            i++;
            if (i < argc)
                main_binary_file = argv[i];
        }
        else if (strcmp(argv[i], "-h") == 0)
        {
            usage();
            return 0;
        }
        else
        {
            printf("Unknown argument: %s\n", argv[i]);
            usage();
            return -1;
        }
    }

    if (in_file == NULL)
    {
        printf("No input file provided\n");
        usage();
        return -1;
    }

    if (out_file == NULL)
    {
        printf("No output file provided\n");
        usage();
        return -1;
    }

    printf("\n");
    printf("Loading main ELF\n");
    printf("----------------\n");
    printf("\n");

    if (main_binary_file == NULL)
    {
        printf("No ELF provided. Skipping.\n");
    }
    else
    {
        // Load symbols and their addresses from the main ELF
        main_binary_load(main_binary_file);
    }

    printf("\n");
    printf("Loading ELF file\n");
    printf("----------------\n");
    printf("\n");

    // Storage for all read sections
    elf_section_info sections[MAX_SECTIONS];
    int read_sections = 0;

    Elf32_Ehdr *hdr = elf_load(in_file);
    if (hdr == NULL)
    {
        printf("Failed to open: %s\n", in_file);
        main_binary_free();
        return -1;
    }

    printf("Looking for sections to include in DSL:\n");

    uint32_t max_address = 0;

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

        // Skip any region that isn't present after loading the ELF file
        //if ((shdr->sh_flags & (SHF_WRITE | SHF_ALLOC | SHF_EXECINSTR)) == 0)
        //    continue;

        uintptr_t address = shdr->sh_addr;

        int type = -1;

        if (strcmp(name, ".nobits") == 0)
            type = DSL_SEGMENT_NOBITS;
        else if (strcmp(name, ".progbits") == 0)
            type = DSL_SEGMENT_PROGBITS;
        else if (strcmp(name, ".rel.progbits") == 0)
            type = DSL_SEGMENT_RELOCATIONS;
        else
            continue;

        void *data;

        if (type == DSL_SEGMENT_PROGBITS)
            data = elf_section_data(hdr, i);
        else if (type == DSL_SEGMENT_RELOCATIONS)
            data = elf_section_data(hdr, i);
        else //if (type == DSL_SEGMENT_NOBITS)
            data = NULL;

        printf("Section %s: 0x%04zX (0x%zX bytes) | Type %d\n",
               name, address, size, type);

        sections[read_sections].address = address;
        sections[read_sections].size = size;
        sections[read_sections].type = type;
        sections[read_sections].data = data;

        uint32_t end_address = address + size;
        if (end_address > max_address)
            max_address = end_address;

        read_sections++;
    }

    printf("Address space size: 0x%X\n", max_address);

    printf("\n");
    printf("Generating symbol table\n");
    printf("-----------------------\n");
    printf("\n");

    int symtab_index = -1;
    int strtab_index = -1;

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
            printf(".strtab section: %d\n", i);
            strtab_index = i;
        }
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

        if (strcmp(name, ".symtab") != 0)
            continue;

        printf(".symtab section: %d\n", i);

        symtab_index = i;

        const Elf32_Sym *sym = elf_section_data(hdr, symtab_index);
        size_t sym_num = size / sizeof(Elf32_Sym);

        printf("Total number of symbols: %zu\n", sym_num);

        for (size_t s = 0; s < sym_num; s++, sym++)
        {
            uint8_t bind = ELF_ST_BIND(sym->st_info);
            uint8_t type = ELF_ST_TYPE(sym->st_info);
            uint8_t vis = ELF_ST_VISIBILITY(sym->st_other);

            const char *name;

            if (type == STT_SECTION)
            {
                const Elf32_Shdr *shdr = elf_section(hdr, sym->st_shndx);
                name = elf_get_string_shstrtab(hdr, shdr->sh_name);
            }
            else
            {
                name = elf_get_string_strtab(hdr, strtab_index, sym->st_name);
            }

            bool public = true;

            // Only save addresses of functions and objects
            if ((type != STT_FUNC) && (type != STT_OBJECT))
                public = false;

            // Only if they are global (not local)
            if (bind != STB_GLOBAL)
                public = false;

            // Only if they are visible from outside of the ELF
            if ((vis != STV_DEFAULT) && (vis != STV_EXPORTED))
                public = false;

            bool unknown = (type == STT_NOTYPE);

            printf("%zu: \"%s\" = %u%s%s\n", s, name, sym->st_value,
                   public ? " [Public]" : "", unknown ? " [Unknown]": "");

            sym_add_to_table(name, sym->st_value, public, unknown);
        }
    }

    printf("\n");
    printf("Generating DSL file\n");
    printf("-------------------\n");
    printf("\n");

    printf("Opening: %s\n", out_file);

    FILE *f_dsl = fopen(out_file, "wb");
    if (f_dsl == NULL)
    {
        printf("Failed to open output file: %s\n", out_file);
        free(hdr);
        main_binary_free();
        return -1;
    }

    // Write header

    dsl_header header = {
        .magic = DSL_MAGIC,
        .version = 0,
        .num_sections = read_sections,
        .unused = {0},
        .addr_space_size = max_address,
    };

    if (fwrite(&header, sizeof(dsl_header), 1, f_dsl) != 1)
    {
        printf("Failed to write DSL header\n");
        goto error;
    }

    // Check relocations to see that there are unsupported types

    int progbits_index = -1;

    for (int i = 0; i < read_sections; i++)
    {
        if (sections[i].type == DSL_SEGMENT_PROGBITS)
        {
            progbits_index = i;
            break;
        }
    }

    if (progbits_index == -1)
    {
        printf("Can't find progbits section to apply relocations\n");
        goto error;
    }

    for (int i = 0; i < read_sections; i++)
    {
        if (sections[i].type != DSL_SEGMENT_RELOCATIONS)
            continue;

        printf("Checking relocations\n");

        Elf32_Rel *rel = sections[i].data;
        size_t num_rel = sections[i].size / sizeof(Elf32_Rel);

        // First, check that we only have valid relocations and mark all the
        // symbols that are referenced by relocations
        for (size_t r = 0; r < num_rel; r++)
        {
            uint8_t type = rel[r].r_info & 0xFF;
            uint8_t symbol_index = rel[r].r_info >> 8;

            if ((type != R_ARM_ABS32) && (type != R_ARM_THM_CALL) &&
                (type != R_ARM_CALL))
            {
                printf("Invalid relocation. Index %zu. Type %u\n", r, type);
                goto error;
            }

            sym_set_as_used(symbol_index);
        }

        // Remove unused symbols and sort them by name

        sym_clear_unused();

        printf("Sorting symbol table...\n");

        sym_sort_table();

        sym_print_table();

        // Now save each relocation replacing the symbol index by the new
        // index in the reduced table.

        for (size_t r = 0; r < num_rel; r++)
        {
            uint32_t offset = rel[r].r_offset;
            uint8_t type = rel[r].r_info & 0xFF;
            uint8_t old_symbol_index = rel[r].r_info >> 8;

            int new_index = sym_get_sym_index_by_old_index(old_symbol_index);
            if (new_index == -1)
            {
                printf("Failed to translate index for relocation %zu\n", r);
                goto error;
            }

            rel[r].r_offset = offset;
            rel[r].r_info  = type | (new_index << 8);
        }
    }

    // Write section headers

    printf("Writing %d sections\n", read_sections);

    unsigned int full_header_size =
        sizeof(dsl_header) + sizeof(dsl_section_header) * read_sections;

    unsigned int current_section_offset = full_header_size;

    for (int i = 0; i < read_sections; i++)
    {
        uint32_t offset = (sections[i].type == DSL_SEGMENT_NOBITS) ?
                          0 : current_section_offset;

        dsl_section_header section_header = {
            .address = sections[i].address,
            .size = sections[i].size,
            .data_offset = offset,
            .type = sections[i].type,
            .unused = {0},
        };

        if (sections[i].type != DSL_SEGMENT_NOBITS)
        {
            printf("Section %d: offset 0x%X, 0x%X bytes\n",
                   i, offset, sections[i].size);

            current_section_offset += sections[i].size;
        }
        else
        {
            printf("Section %d: nobits, 0x%X bytes\n", i, sections[i].size);
        }

        if (fwrite(&section_header, sizeof(dsl_section_header), 1, f_dsl) != 1)
        {
            printf("Failed to write DSL header for section %d\n", i);
            goto error;
        }
    }

    // Write section data

    for (int i = 0; i < read_sections; i++)
    {
        if (sections[i].type == DSL_SEGMENT_NOBITS)
        {
            // Nothing to write to the file
        }
        else if (sections[i].type == DSL_SEGMENT_PROGBITS)
        {
            printf("Writing data of section %d (progbits)\n", i);

            if (fwrite(sections[i].data, sections[i].size, 1, f_dsl) != 1)
            {
                printf("Failed to write DSL data for section %d\n", i);
                goto error;
            }
        }
        else if (sections[i].type == DSL_SEGMENT_RELOCATIONS)
        {
            printf("Writing data of section %d (relocations)\n", i);

            if (fwrite(sections[i].data, sections[i].size, 1, f_dsl) != 1)
            {
                printf("Failed to write DSL data for section %d\n", i);
                goto error;
            }
        }
    }

    // Save symbol table to file

    printf("Saving symbol table...\n");

    if (sym_table_save_to_file(f_dsl) != 0)
    {
        printf("Failed to save symbol table!\n");
        goto error;
    }

    sym_clear_table();

    fclose(f_dsl);

    printf("\n");
    printf("Freeing ELF files\n");
    printf("-----------------\n");
    printf("\n");

    free(hdr);
    main_binary_free();

    return 0;

error:
    free(hdr);
    main_binary_free();
    fclose(f_dsl);
    remove(out_file);
    return -1;
}
