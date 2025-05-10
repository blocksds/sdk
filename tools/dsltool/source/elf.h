// SPDX-License-Identifier: Zlib
//
// Copyright (C) 2025 Antonio Niño Díaz

#ifndef ELF_H__
#define ELF_H__

#include <stdio.h>
#include <stdint.h>

// Types for use within ELF.
typedef uint32_t Elf32_Addr;
typedef uint16_t Elf32_Half;
typedef uint8_t  Elf_Byte;
typedef uint32_t Elf32_Off;
typedef int32_t  Elf32_Sword;
typedef uint32_t Elf32_Word;

// Identification indices.
typedef enum
{
    EI_MAG0    = 0, // File identification.
    EI_MAG1    = 1, // File identification.
    EI_MAG2    = 2, // File identification.
    EI_MAG3    = 3, // File identification.
    EI_CLASS   = 4, // File class.
    EI_DATA    = 5, // Data encoding.
    EI_VERSION = 6, // File version.
    EI_PAD     = 7, // Start of padding bytes.
    EI_NIDENT  = 16 // Size of e_ident[].
} ELF_IDENT;

// Header structure.
typedef struct
{
    unsigned char e_ident[EI_NIDENT]; // Identification bytes.
    Elf32_Half    e_type;             // Object file type.
    Elf32_Half    e_machine;          // Object architecture.
    Elf32_Word    e_version;          // Object file version.
    Elf32_Addr    e_entry;            // Object entry point.
    Elf32_Off     e_phoff;            // Program header file offset.
    Elf32_Off     e_shoff;            // Section header file offset.
    Elf32_Word    e_flags;            // Processor-specific flags.
    Elf32_Half    e_ehsize;           // ELF header size.
    Elf32_Half    e_phentsize;        // Program header entry size.
    Elf32_Half    e_phnum;            // Program header entries.
    Elf32_Half    e_shentsize;        // Section header entry size.
    Elf32_Half    e_shnum;            // Section header entries.
    Elf32_Half    e_shstrndx;         // String table index.
} Elf32_Ehdr;

// Program header structure.
typedef struct
{
    Elf32_Word p_type;   // Segment type.
    Elf32_Off  p_offset; // File offset.
    Elf32_Addr p_vaddr;  // Virtual address.
    Elf32_Addr p_paddr;  // Physical address.
    Elf32_Word p_filesz; // File image size.
    Elf32_Word p_memsz;  // Memory image size.
    Elf32_Word p_flags;  // Segment flags.
    Elf32_Word p_align;  // Alignment value.
} Elf32_Phdr;

// Object file type.
typedef enum
{
    ET_NONE   = 0,  // No file type.
    ET_REL    = 1,  // Relocatable file.
    ET_EXEC   = 2,  // Executable file.
    ET_DYN    = 3,  // Shared object file.
    ET_CORE   = 4,  // Core file.
} ELF_TYPE;

// Object architecture.
typedef enum
{
    EM_NONE  = 0,  // No machine.
    EM_M32   = 1,  // AT&T WE 32100.
    EM_SPARC = 2,  // SPARC.
    EM_386   = 3,  // Intel 80386.
    EM_68K   = 4,  // Motorola 68000.
    EM_88K   = 5,  // Motorola 88000.
    EM_860   = 7,  // Intel 80860.
    EM_MIPS  = 8,  // MIPS RS3000.
    EM_ARM   = 40, // ARM.
} ELF_MACHINE;

// Object file version.
typedef enum
{
    EV_NONE    = 0, // Invalid version.
    EV_CURRENT = 1  // Current version.
} ELF_VERSION;

// Magic number.
#define ELF_MAGIC "\x7f" "ELF"

// File class.
typedef enum
{
    ELFCLASSNONE = 0, // Invalid class.
    ELFCLASS32   = 1, // 32-bit objects.
    ELFCLASS64   = 2, // 64-bit objects.
} ELF_CLASS;

// Data encoding.
typedef enum
{
    ELFDATANONE = 0, // Invalid data encoding.
    ELFDATA2LSB = 1, // Little endian.
    ELFDATA2MSB = 2, // Big endian.
} ELF_DATA;

// Program header segment type.
typedef enum
{
    PT_NULL    = 0, // Unused.
    PT_LOAD    = 1, // Loadable segment.
    PT_DYNAMIC = 2, // Dynamic linking information.
    PT_INTERP  = 3, // Interpreter.
    PT_NOTE    = 4, // Auxiliary information.
    PT_SHLIB   = 5, // Reserved.
    PT_PHDR    = 6  // Program header table.
} ELF_P_TYPE;

// Program header flag.
typedef enum
{
    PF_R        = 4, // Read flag.
    PF_W        = 2, // Write flag.
    PF_X        = 1, // Execute flag.
    PF_MASKPROC = 0xf0000000
} ELF_P_FLAG;

// Section header entry.
typedef struct {
    uint32_t   sh_name;
    uint32_t   sh_type;
    uint32_t   sh_flags;
    Elf32_Addr sh_addr;
    Elf32_Off  sh_offset;
    uint32_t   sh_size;
    uint32_t   sh_link;
    uint32_t   sh_info;
    uint32_t   sh_addralign;
    uint32_t   sh_entsize;
} Elf32_Shdr;

typedef enum {
    SHN_UNDEF     = 0,
    SHN_LORESERVE = 0xff00,
    SHN_LOPROC    = 0xff00,
    SHN_BEFORE    = 0xff00,
    SHN_AFTER     = 0xff01,
    SHN_HIPROC    = 0xff1f,
    SHN_LOOS      = 0xff20,
    SHN_HIOS      = 0xff3f,
    SHN_ABS       = 0xfff1,
    SHN_COMMON    = 0xfff2,
    SHN_XINDEX    = 0xffff,
    SHN_HIRESERVE = 0xffff
} ELF_SPECIAL_SECTION_INDICES;

typedef enum {
    SHT_NULL          = 0,
    SHT_PROGBITS      = 1,
    SHT_SYMTAB        = 2,
    SHT_STRTAB        = 3,
    SHT_RELA          = 4,
    SHT_HASH          = 5,
    SHT_DYNAMIC       = 6,
    SHT_NOTE          = 7,
    SHT_NOBITS        = 8,
    SHT_REL           = 9,
    SHT_SHLIB         = 10,
    SHT_DYNSYM        = 11,
    SHT_INIT_ARRAY    = 14,
    SHT_FINI_ARRAY    = 15,
    SHT_PREINIT_ARRAY = 16,
    SHT_GROUP         = 17,
    SHT_SYMTAB_SHNDX  = 18,
    SHT_LOOS          = 0x60000000,
    SHT_LOSUNW        = 0x6ffffff7,
    SHT_HISUNW        = 0x6fffffff,
    SHT_HIOS          = 0x6fffffff,
    SHT_LOPROC        = 0x70000000,
    SHT_SPARC_GOTDATA = 0x70000000,
    SHT_ARM_ATTRIB    = 0x70000003,
    SHT_HIPROC        = 0x7fffffff,
    SHT_LOUSER        = 0x80000000,
    SHT_HIUSER        = 0xffffffff
} ELF_S_TYPE;

typedef enum {
    SHF_WRITE            = 0x1, // Writable during process execution.
    SHF_ALLOC            = 0x2, // Occupies memory during process execution.
    SHF_EXECINSTR        = 0x4, // Contains executable machine instructions.
    SHF_MERGE            = 0x10,
    SHF_STRINGS          = 0x20,
    SHF_INFO_LINK        = 0x40,
    SHF_LINK_ORDER       = 0x80,
    SHF_OS_NONCONFORMING = 0x100,
    SHF_GROUP            = 0x200,
    SHF_TLS              = 0x400,
    SHF_MASKOS           = 0x0ff00000,
    SHF_ORDERED          = 0x40000000,
    SHF_EXCLUDE          = 0x80000000,
    SHF_MASKPROC         = 0xf0000000
} ELF_S_FLAG;

typedef struct {
    Elf32_Addr r_offset; // Location (virtual address)
    Elf32_Word r_info;   // (Symbol table index << 8) | (type of relocation)
} Elf32_Rel;

#define R_ARM_NONE          0
#define R_ARM_ABS32         2
#define R_ARM_REL32         3
#define R_ARM_THM_CALL      10
#define R_ARM_BASE_PREL     25
#define R_ARM_GOT_BREL      26
#define R_ARM_CALL          28
#define R_ARM_JUMP24        29
#define R_ARM_THM_JUMP24    30
#define R_ARM_TARGET1       38
#define R_ARM_TLS_IE32      107
#define R_ARM_TLS_LE32      108

typedef struct {
    Elf32_Word  st_name;  // Symbol name (.strtab index)
    Elf32_Word  st_value; // Value of symbol
    Elf32_Word  st_size;  // Size of symbol
    Elf_Byte    st_info;  // Type / binding attrs
    Elf_Byte    st_other; // Visibility
    Elf32_Half  st_shndx; // Section index of symbol
} Elf32_Sym;

// Symbol Table index of the undefined symbol
#define ELF_SYM_UNDEFINED   0

// Undefined index
#define STN_UNDEF           0

// st_info: Symbol Bindings
#define STB_LOCAL       0   // Local symbol
#define STB_GLOBAL      1   // Global symbol
#define STB_WEAK        2   // Weakly defined global symbol
#define STB_NUM         3

#define STB_LOOS        10  // Operating system specific range
#define STB_HIOS        12
#define STB_LOPROC      13  // Processor-specific range
#define STB_HIPROC      15

// st_info: Symbol Types
#define STT_NOTYPE      0   // Type not specified
#define STT_OBJECT      1   // Associated with a data object
#define STT_FUNC        2   // Associated with a function
#define STT_SECTION     3   // Associated with a section
#define STT_FILE        4   // Associated with a file name
#define STT_COMMON      5   // Uninitialised common block
#define STT_TLS         6   // Thread local data object
#define STT_NUM         7

// st_other: Visibility Types
#define STV_DEFAULT     0   // Default binding type
#define STV_INTERNAL    1   // Not referenced from outside
#define STV_HIDDEN      2   // Not visible, may be used via ptr
#define STV_PROTECTED   3   // Visible, not preemptible
#define STV_EXPORTED    4
#define STV_SINGLETON   5
#define STV_ELIMINATE   6

// st_info/st_other utility macros
#define ELF_ST_BIND(info)           ((uint32_t)(info) >> 4)
#define ELF_ST_TYPE(info)           ((uint32_t)(info) & 0xf)
#define ELF_ST_INFO(bind, type)     ((Elf_Byte)(((bind) << 4) | ((type) & 0xf)))
#define ELF_ST_VISIBILITY(other)    ((uint32_t)(other) & 3)

static inline const Elf32_Phdr *elf_phdr(Elf32_Ehdr *hdr)
{
    return (Elf32_Phdr *)((uintptr_t)hdr + hdr->e_phoff);
}

static inline const Elf32_Phdr *elf_program(Elf32_Ehdr *hdr, int idx)
{
    return &elf_phdr(hdr)[idx];
}

static inline const void *elf_program_data(Elf32_Ehdr *hdr, int idx)
{
    return ((char *)hdr) + elf_phdr(hdr)[idx].p_offset;
}

static inline const Elf32_Shdr *elf_shdr(Elf32_Ehdr *hdr)
{
    return (Elf32_Shdr *)((uintptr_t)hdr + hdr->e_shoff);
}

static inline const Elf32_Shdr *elf_section(Elf32_Ehdr *hdr, int idx)
{
    return &elf_shdr(hdr)[idx];
}

static inline void *elf_section_data(Elf32_Ehdr *hdr, int idx)
{
    return ((char *)hdr) + elf_shdr(hdr)[idx].sh_offset;
}

static inline const char *elf_get_string_shstrtab(Elf32_Ehdr *hdr, int offset)
{
    if (hdr->e_shstrndx == SHN_UNDEF)
        return NULL;

    char *str_base = (char *)hdr + elf_section(hdr, hdr->e_shstrndx)->sh_offset;
    if (str_base == NULL)
        return NULL;

    return str_base + offset;
}

static inline const char *elf_get_string_strtab(Elf32_Ehdr *hdr, int strtabndx, int offset)
{
    char *str_base = (char *)hdr + elf_section(hdr, strtabndx)->sh_offset;
    if (str_base == NULL)
        return NULL;

    return str_base + offset;
}

Elf32_Ehdr *elf_load(const char *path);

#endif // ELF_H__
