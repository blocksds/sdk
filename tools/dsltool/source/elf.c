// SPDX-License-Identifier: Zlib
//
// Copyright (C) 2025 Antonio Niño Díaz

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "elf.h"
#include "log.h"

static void *file_load(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (f == NULL)
    {
        ERROR("%s couldn't be opened!", filename);
        exit(EXIT_FAILURE);
    }

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    if (size == 0)
    {
        ERROR("Size of %s is 0!", filename);
        fclose(f);
        exit(EXIT_FAILURE);
    }

    rewind(f);
    void *buffer = malloc(size);
    if (buffer == NULL)
    {
        ERROR("Not enought memory to load %s!", filename);
        fclose(f);
        exit(EXIT_FAILURE);
    }

    if (fread(buffer, size, 1, f) != 1)
    {
        ERROR("Error while reading: %s", filename);
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

    INFO("File loaded: %s\n", path);

    if (memcmp(&(header->e_ident[EI_MAG0]), ELF_MAGIC, 4))
    {
        ERROR("Invalid header magic\n");
        goto cleanup;
    }

    if (header->e_ident[EI_CLASS] != ELFCLASS32)
    {
        ERROR("Not a 32-bit ELF file\n");
        goto cleanup;
    }

    if (header->e_ident[EI_DATA] != ELFDATA2LSB)
    {
        ERROR("Not a little-endian ELF file\n");
        goto cleanup;
    }

    if (header->e_ident[EI_VERSION] != EV_CURRENT)
    {
        ERROR("Invalid ELF version\n");
        goto cleanup;
    }

    if (header->e_type != ET_EXEC)
    {
        ERROR("ELF file isn't executable\n");
        goto cleanup;
    }

    if (header->e_machine != EM_ARM)
    {
        ERROR("ELF architecture isn't ARM\n");
        goto cleanup;
    }

    if (header->e_ehsize != sizeof(Elf32_Ehdr))
    {
        ERROR("Invalid ELF header size\n");
        goto cleanup;
    }

    if (header->e_phnum == 0)
    {
        ERROR("No program headers\n");
        goto cleanup;
    }

    if (header->e_shnum == 0)
    {
        ERROR("No section headers\n");
        goto cleanup;
    }

    VERBOSE("ELF header OK!\n");

    VERBOSE("ELF entrypoint: 0x%04X\n", (unsigned int)header->e_entry);

    VERBOSE("%u programs:\n", (unsigned int)header->e_phnum);

    // Iterate over each program header
    for (unsigned int i = 0; i < header->e_phnum; i++)
    {
        const Elf32_Phdr *phdr = elf_program(header, i);

        if (phdr->p_vaddr != phdr->p_paddr)
        {
            VERBOSE("Physical and virtual addresses are different: 0x%X != 0x%X\n",
                    (unsigned int)phdr->p_vaddr, (unsigned int)phdr->p_paddr);
        }

        if (phdr->p_filesz != phdr->p_memsz)
        {
            VERBOSE("File and memory sizes are different: 0x%X != 0x%X\n",
                    (unsigned int)phdr->p_filesz, (unsigned int)phdr->p_memsz);
        }

        VERBOSE("%u: Address: 0x%04X | Size: 0x%04X | ",
                i, (unsigned int)phdr->p_paddr, (unsigned int)phdr->p_memsz);

        VERBOSE("%c", phdr->p_flags & PF_R ? 'R' : '-');
        VERBOSE("%c", phdr->p_flags & PF_W ? 'W' : '-');
        VERBOSE("%c", phdr->p_flags & PF_X ? 'X' : '-');

        if (phdr->p_type == PT_NULL)
            VERBOSE(" | NULL");
        else if (phdr->p_type == PT_LOAD)
            VERBOSE(" | LOAD");
        else if (phdr->p_type == PT_DYNAMIC)
            VERBOSE(" | DYNAMIC");
        else if (phdr->p_type == PT_INTERP)
            VERBOSE(" | INTERP");
        else if (phdr->p_type == PT_NOTE)
            VERBOSE(" | NOTE");
        else if (phdr->p_type == PT_SHLIB)
            VERBOSE(" | SHLIB");
        else if (phdr->p_type == PT_PHDR)
            VERBOSE(" | PHDR");
        else
            VERBOSE(" | %d", phdr->p_type);

        VERBOSE("\n");

        // Skip non-static sections.
        //if (phdr->p_flags & 0x200000)
        //    continue;
    }

    VERBOSE("%u sections:\n", (unsigned int)header->e_shnum);

    // Iterate over each section header
    for (unsigned int i = 0; i < header->e_shnum; i++)
    {
        const Elf32_Shdr *shdr = elf_section(header, i);

        VERBOSE("%u: Address: 0x%08X | Size: 0x%04X | ",
                i, (unsigned int)shdr->sh_addr, (unsigned int)shdr->sh_size);

        VERBOSE("%c", shdr->sh_flags & SHF_WRITE ? 'W' : '-');
        VERBOSE("%c", shdr->sh_flags & SHF_ALLOC ? 'A' : '-');
        VERBOSE("%c", shdr->sh_flags & SHF_EXECINSTR ? 'X' : '-');

        const char *name = elf_get_string_shstrtab(header, shdr->sh_name);
        VERBOSE(" | %s : ", name == NULL ? "(empty)" : name);

        if (shdr->sh_type == SHT_NULL)
            VERBOSE("NULL");
        else if (shdr->sh_type == SHT_PROGBITS)
            VERBOSE("PROGBITS");
        else if (shdr->sh_type == SHT_SYMTAB)
            VERBOSE("SYMTAB");
        else if (shdr->sh_type == SHT_STRTAB)
            VERBOSE("STRTAB");
        else if (shdr->sh_type == SHT_RELA)
            VERBOSE("RELA");
        else if (shdr->sh_type == SHT_HASH)
            VERBOSE("HASH");
        else if (shdr->sh_type == SHT_DYNAMIC)
            VERBOSE("DYNAMIC");
        else if (shdr->sh_type == SHT_NOTE)
            VERBOSE("NOTE");
        else if (shdr->sh_type == SHT_NOBITS)
            VERBOSE("NOBITS");
        else if (shdr->sh_type == SHT_REL)
            VERBOSE("REL");
        else if (shdr->sh_type == SHT_SHLIB)
            VERBOSE("SHLIB");
        else if (shdr->sh_type == SHT_DYNSYM)
            VERBOSE("DYNSYM");
        else if (shdr->sh_type == SHT_INIT_ARRAY)
            VERBOSE("INIT_ARRAY");
        else if (shdr->sh_type == SHT_FINI_ARRAY)
            VERBOSE("FINI_ARRAY");
        else if (shdr->sh_type == SHT_PREINIT_ARRAY)
            VERBOSE("PREINIT_ARRAY");
        else if (shdr->sh_type == SHT_GROUP)
            VERBOSE("GROUP");
        else if (shdr->sh_type == SHT_SYMTAB_SHNDX)
            VERBOSE("SYMTAB_SHNDX");
        else if (shdr->sh_type == SHT_ARM_ATTRIB)
            VERBOSE("ARM_ATTRIB");
        else
            VERBOSE("%X", (unsigned int)shdr->sh_type);

        VERBOSE("\n");
    }

    return header;

cleanup:
    free(header);
    return NULL;
}
