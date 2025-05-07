// SPDX-License-Identifier: Zlib
//
// Copyright (C) 2025 Antonio Niño Díaz

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "elf.h"
#include "dsl.h"
#include "log.h"
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
    INFO("Usage: dsltool -i input.elf -o output.dsl [-m main_binary.elf] [-v]\n"
         "\n"
         "  -i input.elf       ELF file of the dynamic library.\n"
         "  -o output.dsl      Path to DSL file to be created.\n"
         "  -m main_binary.elf Optional main binary ELF file to resolve symbols\n"
         "  -v                 Enable verbose logging\n"
    );
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
    INFO("dsltool " PACKAGE_VERSION "\n"
         "=============\n"
    );

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
        else if (strcmp(argv[i], "-v") == 0)
        {
            set_log_level(LOG_VERBOSE);
        }
        else
        {
            ERROR("Unknown argument: %s\n", argv[i]);
            usage();
            return -1;
        }
    }

    if (in_file == NULL)
    {
        ERROR("No input file provided\n");
        usage();
        return -1;
    }

    if (out_file == NULL)
    {
        ERROR("No output file provided\n");
        usage();
        return -1;
    }

    VERBOSE("\n"
            "Loading main ELF\n"
            "----------------\n"
            "\n");

    if (main_binary_file == NULL)
    {
        INFO("No main binary ELF provided. Skipping.\n");
    }
    else
    {
        // Load symbols and their addresses from the main ELF
        main_binary_load(main_binary_file);
    }

    VERBOSE("\n"
            "Loading ELF file\n"
            "----------------\n"
            "\n");

    // Storage for all read sections
    elf_section_info sections[MAX_SECTIONS];
    int read_sections = 0;

    Elf32_Ehdr *hdr = elf_load(in_file);
    if (hdr == NULL)
    {
        ERROR("Failed to open: %s\n", in_file);
        main_binary_free();
        return -1;
    }

    VERBOSE("Looking for sections to include in DSL:\n");

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

        VERBOSE("Section %s: 0x%04zX (0x%zX bytes) | Type %d\n",
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

    INFO("Address space size: 0x%X\n", max_address);

    VERBOSE("\n"
            "Generating symbol table\n"
            "-----------------------\n"
            "\n");

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
            VERBOSE(".strtab section: %d\n", i);
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

        VERBOSE(".symtab section: %d\n", i);

        symtab_index = i;

        const Elf32_Sym *sym = elf_section_data(hdr, symtab_index);
        size_t sym_num = size / sizeof(Elf32_Sym);

        VERBOSE("Total number of symbols: %zu\n", sym_num);

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

            // Symbols without a type are unknown. However, any symbol in a TLS
            // section must refer to the main binary as well, because a dynamic
            // library can't have TLS sections with the current codebase.
            bool unknown = (type == STT_NOTYPE) || (type == STT_TLS);

            VERBOSE("%zu: \"%s\" = %u%s%s\n", s, name, sym->st_value,
                    public ? " [Public]" : "", unknown ? " [Unknown]": "");

            sym_add_to_table(name, sym->st_value, public, unknown);
        }
    }

    VERBOSE("\n"
            "Generating DSL file\n"
            "-------------------\n"
            "\n");

    INFO("Creating file: %s\n", out_file);

    FILE *f_dsl = fopen(out_file, "wb");
    if (f_dsl == NULL)
    {
        ERROR("Failed to open output file: %s\n", out_file);
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
        ERROR("Failed to write DSL header\n");
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
        ERROR("Can't find progbits section to apply relocations\n");
        goto error;
    }

    bool symbols_cleared = false;

    for (int i = 0; i < read_sections; i++)
    {
        if (sections[i].type != DSL_SEGMENT_RELOCATIONS)
            continue;

        VERBOSE("Checking relocations\n");

        Elf32_Rel *rel = sections[i].data;
        size_t num_rel = sections[i].size / sizeof(Elf32_Rel);

        // First, check that we only have valid relocations and mark all the
        // symbols that are referenced by relocations
        for (size_t r = 0; r < num_rel; r++)
        {
            uint8_t type = rel[r].r_info & 0xFF;
            int symbol_index = rel[r].r_info >> 8;

            if ((type == R_ARM_ABS32) || (type == R_ARM_THM_CALL) ||
                (type == R_ARM_CALL) || (type == R_ARM_TLS_LE32))
            {
                sym_set_as_used(symbol_index);
            }
            else
            {
                ERROR("Invalid relocation. Index %zu. Type %u\n", r, type);
                goto error;
            }
        }

        // Remove unused symbols and sort them by name

        sym_clear_unused();

        VERBOSE("Sorting symbol table...\n");

        sym_sort_table();

        symbols_cleared = true;

        sym_print_table();

        // Now save each relocation replacing the symbol index by the new
        // index in the reduced table.

        for (size_t r = 0; r < num_rel; r++)
        {
            uint32_t offset = rel[r].r_offset;
            uint8_t type = rel[r].r_info & 0xFF;
            int old_symbol_index = rel[r].r_info >> 8;

            int new_index = sym_get_sym_index_by_old_index(old_symbol_index);
            if (new_index == -1)
            {
                ERROR("Failed to translate index for relocation %zu\n", r);
                goto error;
            }

            rel[r].r_offset = offset;
            rel[r].r_info  = type | (new_index << 8);
        }
    }

    if (!symbols_cleared)
    {
        sym_clear_unused();

        VERBOSE("Sorting symbol table...\n");

        sym_sort_table();

        sym_print_table();
    }

    // Write section headers

    VERBOSE("Writing %d sections\n", read_sections);

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
            VERBOSE("Section %d: offset 0x%X, 0x%X bytes\n",
                   i, offset, sections[i].size);

            current_section_offset += sections[i].size;
        }
        else
        {
            VERBOSE("Section %d: nobits, 0x%X bytes\n", i, sections[i].size);
        }

        if (fwrite(&section_header, sizeof(dsl_section_header), 1, f_dsl) != 1)
        {
            ERROR("Failed to write DSL header for section %d\n", i);
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
            VERBOSE("Writing data of section %d (progbits)\n", i);

            if (fwrite(sections[i].data, sections[i].size, 1, f_dsl) != 1)
            {
                ERROR("Failed to write DSL data for section %d\n", i);
                goto error;
            }
        }
        else if (sections[i].type == DSL_SEGMENT_RELOCATIONS)
        {
            VERBOSE("Writing data of section %d (relocations)\n", i);

            if (fwrite(sections[i].data, sections[i].size, 1, f_dsl) != 1)
            {
                ERROR("Failed to write DSL data for section %d\n", i);
                goto error;
            }
        }
    }

    // Save symbol table to file

    VERBOSE("Saving symbol table...\n");

    if (sym_table_save_to_file(f_dsl) != 0)
    {
        ERROR("Failed to save symbol table!\n");
        goto error;
    }

    sym_clear_table();

    fclose(f_dsl);

    VERBOSE("\n"
            "Freeing ELF files\n"
            "-----------------\n"
            "\n");

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
