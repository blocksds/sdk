// SPDX-License-Identifier: Zlib
//
// Copyright (C) 2025 Antonio Niño Díaz

#ifndef LIBNDS_DSL_H__
#define LIBNDS_DSL_H__

#include <assert.h>
#include <stdint.h>

/// DSL (Nintendo DS Loadable Format) description and helpers.
///
/// DSL is a format for shared libraries that can be loaded with BlocksDS. It is
/// a simplified version of the ELF format used in .so files.
///
/// General structure of the file:
///
///     +======================+
///     | DSL header           | It specifies the number of sections in the file.
///     +======================+
///     | DSL section header 0 | As many section headers as sections.
///     +----------------------+
///     | DSL section header 1 |
///     +----------------------+
///     | DSL section header 2 |
///     +======================+
///     | Section data 0       | Data of each section, one after the other.
///     +----------------------+
///     | Section data 1       |
///     +----------------------+
///     | Section data 2       |
///     +======================+
///     | Symbol table         |
///     +----------------------+
///     | Symbol table names   |
///     +======================+
///
/// DSL header: General information about the file.
///
///     +--------------------+-------------+--------------------------------+
///     | Field              | Type        | Notes                          |
///     +====================+=============+================================+
///     | Magic              | uint32_t    | 0x304C5344 == 'DSL0'           |
///     +--------------------+-------------+--------------------------------+
///     | Version            | uint8_t     | Current version: 0             |
///     +--------------------+-------------+--------------------------------+
///     | Number of sections | uint8_t     |                                |
///     +--------------------+-------------+--------------------------------+
///     | Unused             | uint8_t[2]  | Unused, set to zero.           |
///     +--------------------+-------------+--------------------------------+
///     | Address space size | uint32_t    | From 0 to the max address.     |
///     +====================+=============+================================+
///
/// DSL section header: Saved right after the DSL header. This is repeated for
/// all sections stored in the file.
///
///     +====================+=============+================================+
///     | Address            | uint16_t    | As seen by the DSP (in words). |
///     +--------------------+-------------+--------------------------------+
///     | Size (in bytes)    | uint16_t    |                                |
///     +--------------------+-------------+--------------------------------+
///     | Section type       | uint8_t     | See DSL_SEGMENT_*              |
///     +--------------------+-------------+--------------------------------+
///     | Unused             | uint8_t[3]  | Unused, set to zero.           |
///     +--------------------+-------------+--------------------------------+
///     | Data offset        | uint32_t    | Offset to the section data     |
///     |                    |             | from the start of the file.    |
///     +====================+=============+================================+
///
/// Section data: The data of the sections is stored right after the array of
/// DSL section headers.

/// DSL section header description
typedef struct {
    uint32_t address;       ///< Address to load the section to
    uint32_t size;          ///< Size in bytes
    uint32_t data_offset;   ///< Offset of the file to the data of the section
    uint8_t type;           ///< DSL_SEGMENT_CODE or DSL_SEGMENT_DATA
    uint8_t unused[3];      ///< Unused. Set to zero
} dsl_section_header;

static_assert(sizeof(dsl_section_header) == 16);

#define DSL_SEGMENT_NOBITS      0
#define DSL_SEGMENT_PROGBITS    1
#define DSL_SEGMENT_RELOCATIONS 2

/// DSL file header
typedef struct {
    uint32_t magic;             ///< Magic number: DSL_MAGIC
    uint8_t version;            ///< Version number (currently 0)
    uint8_t num_sections;       ///< Number of sections in the file
    uint8_t unused[2];          ///< Unused. Set to zero
    uint32_t addr_space_size;   ///< Size of the address space used by the DSL file
    dsl_section_header section[]; ///< Array of section headers
} dsl_header;

/// Magic value of the DSL header file. Same as 'DSL0'
#define DSL_MAGIC 0x304C5344

static_assert(sizeof(dsl_header) == 12);

/// DSL Symbol table: Saved right after the DSL section data.
///
///     +====================+=============+================================+
///     | Number of symbols  | uint16_t    | Number of symbols.             |
///     +--------------------+-------------+--------------------------------+
///     | Unused             | uint8_t[2]  | Unused, set to zero.           |
///     +====================+=============+================================+
///
/// Symbol data: Repeated once per symbol, stored right after the header.
///
///     +====================+=============+================================+
///     | Name string offset | uint32_t    | Offset to name (from the       |
///     |                    |             | symbol table header).          |
///     +--------------------+-------------+--------------------------------+
///     | Value              | uint32_t    | Address of the symbol.         |
///     +--------------------+-------------+--------------------------------+
///     | Attributes         | uint32_t    | Attributes (DSL_SYMBOL_*).     |
///     +====================+=============+================================+
///
/// This is followed by a series of NUL-terminated strings (the symbol names).

#define DSL_SYMBOL_PUBLIC       1 ///< If not set, the symbol is private
#define DSL_SYMBOL_MAIN_BINARY  2 ///< If set, the symbol is in the main binary

/// DSL symbol
typedef struct {
    uint32_t name_str_offset; ///< Offset to symbol name from symbol table base
    uint32_t value;           ///< Address of the symbol
    uint32_t attributes;      ///< Attributes of the symbol (DSL_SYMBOL_*).
} dsl_symbol;

static_assert(sizeof(dsl_symbol) == 12);

/// DSL symbol table
typedef struct {
    uint16_t num_symbols; ///< Number of symbols in the file
    uint8_t unused[2];    ///< Unused. Set to zero
    dsl_symbol symbol[];  ///< Array of symbols
} dsl_symbol_table;

static_assert(sizeof(dsl_symbol_table) == 4);

#endif // LIBNDS_DSL_H__
