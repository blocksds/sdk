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
        __bothinit_array_start = .;
        /* Start with the preinit array */
        KEEP (*(.preinit_array))
        /* Continue with the init array */
        KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
        KEEP (*(.init_array .ctors))
        __bothinit_array_end = .;

        /* End with the fini array */
        __fini_array_start = .;
        KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
        KEEP (*(.fini_array .dtors))
        /* Required by pico-exitprocs.c. */
        KEEP (*(.fini_array*))
        __fini_array_end = .;

        /* Each module must have its own instance of this symbol. */
        PROVIDE_HIDDEN (__dso_handle = .);
        LONG(0);

        /* Now, place all regular code and data */
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
