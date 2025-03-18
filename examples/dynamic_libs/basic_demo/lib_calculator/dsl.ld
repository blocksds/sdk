/*
 * SPDX-License-Identifier: Zlib
 *
 * Copyright (C) 2025 Antonio Niño Díaz
 */

OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)

SECTIONS
{
    .progbits ALIGN(4) :
    {
        *(.text*);
        . = ALIGN(4);
        *(.rodata*);
        . = ALIGN(4);
        *(.data*);
        . = ALIGN(4);
    }

    .nobits ALIGN(4) (NOLOAD) :
    {
        *(.bss* COMMON);
        . = ALIGN(4);
    }
}
