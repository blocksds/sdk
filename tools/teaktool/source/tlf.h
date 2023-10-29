// SPDX-License-Identifier: Zlib
//
// Copyright (C) 2023 Antonio Niño Díaz

#ifndef TLF_H__
#define TLF_H__

#include <stdint.h>

/// TLF (Teak Loadable Format) description and helpers.
///
/// TLF is a format designed to hold executable files for the Teak DSP of the
/// DSi. It is a simplified version of the CDC/DSP1 format.
///
/// General structure of the file:
///
///     +======================+
///     | TLF header           | It specifies the number of sections in the file.
///     +======================+
///     | TLF section header 0 | As many section headers as sections.
///     +----------------------+
///     | TLF section header 1 |
///     +----------------------+
///     | TLF section header 2 |
///     +======================+
///     | Section data 0       | Data of each section, one after the other.
///     +----------------------+
///     | Section data 1       |
///     +----------------------+
///     | Section data 2       |
///     +======================+
///
/// TLF header: General information about the file.
///
///     +--------------------+-------------+--------------------------------+
///     | Field              | Type        | Notes                          |
///     +====================+=============+================================+
///     | Magic              | uint32_t    | 0x30464C54 == 'TLF0'           |
///     +--------------------+-------------+--------------------------------+
///     | Version            | uint8_t     | Current version: 0             |
///     +--------------------+-------------+--------------------------------+
///     | Number of sections | uint8_t     |                                |
///     +--------------------+-------------+--------------------------------+
///     | Padding            | uint8_t * 2 | Unused, set to zero.           |
///     +====================+=============+================================+
///
/// TLF section header: Saved right after the TLF header. This is repeated for
/// all sections stored in the file.
///
///     +====================+=============+================================+
///     | Address            | uint16_t    | As seen by the DSP (in words). |
///     +--------------------+-------------+--------------------------------+
///     | Size (in bytes)    | uint16_t    |                                |
///     +--------------------+-------------+--------------------------------+
///     | Section type       | uint8_t     | 0 = code, 1 = data.            |
///     +--------------------+-------------+--------------------------------+
///     | Padding            | uint8_t * 3 | Unused, set to zero.           |
///     +--------------------+-------------+--------------------------------+
///     | Data offset        | uint32_t    | Offset to the section data     |
///     |                    |             | from the start of the file.    |
///     +====================+=============+================================+
///
/// Section data: The data of the sections is stored right after the array of
/// TLF section headers.

/// TLF section header description
typedef struct {
    uint16_t address;       ///< Address as seen from the DSP (in words)
    uint16_t size;          ///< Size in bytes
    uint8_t type;           ///< TLF_SEGMENT_CODE or TLF_SEGMENT_DATA
    uint8_t padding[3];     ///< Unused. Set to zero
    uint32_t data_offset;   ///< Offset of the file to the data of the section
} tlf_section_header;

static_assert(sizeof(tlf_section_header) == 12);

/// The section contains code
#define TLF_SEGMENT_CODE 0
/// The section contains data
#define TLF_SEGMENT_DATA 1

/// TLF file header
typedef struct {
    uint32_t magic;               ///< Magic number: TLF_MAGIC
    uint8_t version;              ///< Version number (currently 0)
    uint8_t num_sections;         ///< Number of sections in the file
    uint8_t padding[2];           ///< Unused. Set to zero
    tlf_section_header section[]; ///< Array of section headers
} tlf_header;

/// Magic value of the TLF header file. Same as 'TLF0'
#define TLF_MAGIC 0x30464C54

static_assert(sizeof(tlf_header) == 8);

#endif // TLF_H__
