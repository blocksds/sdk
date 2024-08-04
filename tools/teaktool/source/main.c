// SPDX-License-Identifier: MIT
//
// Copyright (C) 2023 Antonio Niño Díaz

#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "elf.h"
#include "tlf.h"

void usage(void)
{
    printf("Usage: teaktool -i input_file.elf -o output_file.tlf\n");
}

typedef struct {
    uint32_t address;
    uint32_t size;
    uint32_t type;
    const void *data;
} elf_section_info;

#define MAX_SECTIONS 10

int main(int argc, char *argv[])
{
    printf("Teak tool " PACKAGE_VERSION "\n");

    const char *in_file = NULL;
    const char *out_file = NULL;

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

    // Storage for all read sections
    elf_section_info sections[MAX_SECTIONS];
    int read_sections = 0;

    Elf32_Ehdr *hdr = elf_load(in_file);

    printf("Looking for loadable sections:\n");

    for (unsigned int i = 0; i < hdr->e_shnum; i++)
    {
        const Elf32_Shdr *shdr = elf_section(hdr, i);

        const char *name = elf_get_string(hdr, shdr->sh_name);
        if (name == NULL)
            continue;

        size_t size = shdr->sh_size;
        if (size == 0)
            continue;

        uint8_t type;

        if (strcmp(name, ".text") == 0)
            type = TLF_SEGMENT_CODE;
        else if (strcmp(name, ".rodata") == 0)
            type = TLF_SEGMENT_DATA;
        else if (strcmp(name, ".data") == 0)
            type = TLF_SEGMENT_DATA;
        else
            continue;

        const void *data = elf_section_data(hdr, i);
        uintptr_t address = (shdr->sh_addr & 0xFFFF) >> 1;

        printf("%s section found: 0x%04zX (0x%zX bytes)\n", name, address, size);

        sections[read_sections].address = address;
        sections[read_sections].size = size;
        sections[read_sections].type = type;
        sections[read_sections].data = data;

        read_sections++;
    }

    // Generate TLF file
    // =================

    printf("Opening %s\n", out_file);

    FILE *f_tlf = fopen(out_file, "wb");
    if (f_tlf == NULL)
    {
        printf("Failed to open output file: %s\n", out_file);
        free(hdr);
        return -1;
    }

    // Write header

    tlf_header header = {
        .magic = TLF_MAGIC,
        .version = 0,
        .num_sections = read_sections,
        .padding = {0}
    };

    if (fwrite(&header, sizeof(tlf_header), 1, f_tlf) != 1)
    {
        printf("Failed to write TLF header\n");
        free(hdr);
        return -1;
    }

    // Write section headers

    printf("Writing %d sections\n", read_sections);

    unsigned int full_header_size =
        sizeof(tlf_header) + sizeof(tlf_section_header) * read_sections;

    unsigned int current_section_offset = full_header_size;

    for (int i = 0; i < read_sections; i++)
    {
        tlf_section_header section_header = {
            .address = sections[i].address,
            .size = sections[i].size,
            .type = sections[i].type,
            .padding = {0},
            .data_offset = current_section_offset
        };

        printf("Section %d placed at offset 0x%X (0x%X bytes)\n", i,
               current_section_offset, sections[i].size);

        current_section_offset += sections[i].size;

        if (fwrite(&section_header, sizeof(tlf_section_header), 1, f_tlf) != 1)
        {
            printf("Failed to write TLF header for section %d\n", i);
            free(hdr);
            return -1;
        }
    }

    // Write section data

    for (int i = 0; i < read_sections; i++)
    {
        if (fwrite(sections[i].data, sections[i].size, 1, f_tlf) != 1)
        {
            printf("Failed to write TLF data for section %d\n", i);
            free(hdr);
            return -1;
        }
    }

    free(hdr);

    return 0;
}
