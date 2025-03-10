// SPDX-License-Identifier: MIT
//
// Copyright (C) 2023 Antonio Niño Díaz

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "elf.h"

static void *file_load(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (f == NULL)
    {
        printf("%s couldn't be opened!", filename);
        exit(EXIT_FAILURE);
    }

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    if (size == 0)
    {
        printf("Size of %s is 0!", filename);
        fclose(f);
        exit(EXIT_FAILURE);
    }

    rewind(f);
    void *buffer = malloc(size);
    if (buffer == NULL)
    {
        printf("Not enought memory to load %s!", filename);
        fclose(f);
        exit(EXIT_FAILURE);
    }

    if (fread(buffer, size, 1, f) != 1)
    {
        printf("Error while reading: %s", filename);
        fclose(f);
        free(buffer);
        exit(EXIT_FAILURE);
    }

    fclose(f);

    return buffer;
}

Elf32_Ehdr *elf_load(const char *path)
{
    // Load the whole ELF file
    Elf32_Ehdr *header = file_load(path);

    printf("File loaded: %s\n", path);

    if (memcmp(&(header->e_ident[EI_MAG0]), ELF_MAGIC, 4))
    {
        printf("Invalid header magic\n");
        goto cleanup;
    }

    if (header->e_ident[EI_CLASS] != ELFCLASS32)
    {
        printf("Not a 32-bit ELF file\n");
        goto cleanup;
    }

    if (header->e_ident[EI_DATA] != ELFDATA2LSB)
    {
        printf("Not a little-endian ELF file\n");
        goto cleanup;
    }

    if (header->e_ident[EI_VERSION] != EV_CURRENT)
    {
        printf("Invalid ELF version\n");
        goto cleanup;
    }

    if (header->e_type != ET_EXEC)
    {
        printf("ELF file isn't executable\n");
        goto cleanup;
    }

    if (header->e_machine != EM_TEAK)
    {
        printf("ELF architecture isn't Teak\n");
        goto cleanup;
    }

    if (header->e_ehsize != sizeof(Elf32_Ehdr))
    {
        printf("Invalid ELF header size\n");
        goto cleanup;
    }

    if (header->e_phnum == 0)
    {
        printf("No program headers\n");
        goto cleanup;
    }

    if (header->e_shnum == 0)
    {
        printf("No section headers\n");
        goto cleanup;
    }

    printf("ELF header OK!\n");

    printf("ELF entrypoint: 0x%04X\n", (unsigned int)header->e_entry);

    printf("%u programs:\n", (unsigned int)header->e_phnum);

    // Iterate over each program header
    for (unsigned int i = 0; i < header->e_phnum; i++)
    {
        const Elf32_Phdr *phdr = elf_program(header, i);

        if (phdr->p_vaddr != phdr->p_paddr)
        {
            printf("Physical and virtual addresses are different: 0x%X != 0x%X\n",
                   (unsigned int)phdr->p_vaddr, (unsigned int)phdr->p_paddr);
        }

        if (phdr->p_filesz != phdr->p_memsz)
        {
            printf("File and memory sizes are different: 0x%X != 0x%X\n",
                   (unsigned int)phdr->p_filesz, (unsigned int)phdr->p_memsz);
        }

        printf("%u: Address: 0x%04X | Size: 0x%04X | ",
               i, (unsigned int)phdr->p_paddr, (unsigned int)phdr->p_memsz);

        printf("%c", phdr->p_flags & PF_R ? 'R' : '-');
        printf("%c", phdr->p_flags & PF_W ? 'W' : '-');
        printf("%c", phdr->p_flags & PF_X ? 'X' : '-');

        if (phdr->p_type == PT_LOAD)
            printf(" | LOAD");
        else
            printf(" | NOLOAD");

        printf("\n");

        // Skip non-static sections.
        //if (phdr->p_flags & 0x200000)
        //    continue;
    }

    printf("%u sections:\n", (unsigned int)header->e_shnum);

    // Iterate over each section header
    for (unsigned int i = 0; i < header->e_shnum; i++)
    {
        const Elf32_Shdr *shdr = elf_section(header, i);

        printf("%u: Address: 0x%08X | Size: 0x%04X | ",
               i, (unsigned int)shdr->sh_addr, (unsigned int)shdr->sh_size);

        printf("%c", shdr->sh_flags & SHF_WRITE ? 'W' : '-');
        printf("%c", shdr->sh_flags & SHF_ALLOC ? 'A' : '-');
        printf("%c", shdr->sh_flags & SHF_EXECINSTR ? 'X' : '-');

        const char *name = elf_get_string(header, shdr->sh_name);
        printf(" | %s : ", name == NULL ? "(empty)" : name);

        if (shdr->sh_type == SHT_PROGBITS)
            printf("PROGBITS");
        else if (shdr->sh_type == SHT_NOBITS)
            printf("NOBITS");
        else if (shdr->sh_type == SHT_NULL)
            printf("NULL");
        else if (shdr->sh_type == SHT_SYMTAB)
            printf("SYMTAB");
        else if (shdr->sh_type == SHT_STRTAB)
            printf("STRTAB");
        else
            printf("%X", (unsigned int)shdr->sh_type);

        printf("\n");
    }

    return header;

cleanup:
    free(header);
    return NULL;
}
