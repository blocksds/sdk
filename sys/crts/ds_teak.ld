/*
 * SPDX-License-Identifier: Zlib
 * SPDX-FileNotice: Modified from the original version by the BlocksDS project.
 *
 * Copyright (C) 2020 Gericom
 */

OUTPUT_FORMAT("elf32-teak", "elf32-teak", "elf32-teak")
OUTPUT_ARCH(teak)

ENTRY(_start)
SECTIONS
{
    . = 0x0;

    .text : ALIGN(4)
    {
        *(.text.start) *(.text*);
        . = ALIGN(4);
    }

    /*
     * The address space of the DSP is 64 kilowords. This will wrap around back
     * to address 0. This is required because the DSP has different address
     * spaces for code and data.
     */
    . = 0x10000000;

    /* The stack is stored at 0x0000-0x0500 */
    . += 0x500 * 2;

    .rodata : AT(SIZEOF(.text)) ALIGN(4)
    {
        *(.rodata*);
        . = ALIGN(4);
    }

    .data : AT(SIZEOF(.text) + SIZEOF(.rodata)) ALIGN(4)
    {
        *(.data.ipc) *(.data.sndregs) *(.data*);
        . = ALIGN(4);
    }

    .bss : AT(SIZEOF(.text) + SIZEOF(.rodata) + SIZEOF(.data)) ALIGN(8)
    {
        __bss_start = .;
        *(.bss* COMMON);
        . = ALIGN(8);
        __bss_end = .;
    }

    . = ALIGN(4);
}
