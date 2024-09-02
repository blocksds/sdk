/* Copyright (C) 2014-2024 Free Software Foundation, Inc.
   Copying and distribution of this script, with or without modification,
   are permitted in any medium without royalty provided the copyright
   notice and this notice are preserved.  */

/* SPDX-License-Identifier: MPL-2.0 AND FSFAP */

OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

MEMORY
{
    /*
     * This base address is chosen to be a reserved instruction in THUMB mode,
     * and SWILT in ARM mode, with an impossible SWI value. This should allow
     * the patcher / linker to find all shifted addresses within each section.
     * Changed sections are: glue_7, got
     */
    ddmem : ORIGIN = 0xBF800000, LENGTH = 16K
}

SECTIONS
{
    .crt0 :
    {
        __text_start = .;
        KEEP (*(.crt0))
        . = ALIGN(4);  /* REQUIRED. LD is flaky without it. */
    } >ddmem = 0xff

    .text : /* ALIGN (4): */
    {
        *(.text.*)
        *(.stub)
        /* .gnu.warning sections are handled specially by elf32.em. */
        *(.gnu.warning)
        *(.gnu.linkonce.t*)
        __glue_start = ABSOLUTE(.);
        *(.glue_7)
        *(.glue_7t)
        __glue_end = ABSOLUTE(.);
        . = ALIGN(4); /* REQUIRED. LD is flaky without it. */
    } >ddmem = 0xff

    .fini :
    {
        KEEP (*(.fini))
        . = ALIGN(4); /* REQUIRED. LD is flaky without it. */
    } >ddmem =0xff

    __text_end = .;

    .rodata :
    {
        *(.rodata)
        *all.rodata*(*)
        *(.roda)
        *(.rodata.*)
        *(.gnu.linkonce.r*)
        SORT(CONSTRUCTORS)
        . = ALIGN(4); /* REQUIRED. LD is flaky without it. */
    } >ddmem = 0xff

    .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >ddmem
    __exidx_start = .;
    .ARM.exidx   : { *(.ARM.exidx* .gnu.linkonce.armexidx.*) } >ddmem
    __exidx_end = .;

/*
 * Ensure the __preinit_array_start label is properly aligned.  We could instead
 * move the label definition inside the section, but the linker would then
 * create the section even if it turns out to be empty, which isn't pretty.
 */

    . = ALIGN(32 / 8);
    PROVIDE (__preinit_array_start = .);
    .preinit_array     : { KEEP (*(.preinit_array)) } >ddmem = 0xff
    PROVIDE (__preinit_array_end = .);
    PROVIDE (__init_array_start = .);
    .init_array     : { KEEP (*(.init_array)) } >ddmem = 0xff
    PROVIDE (__init_array_end = .);
    PROVIDE (__fini_array_start = .);
    .fini_array     : { KEEP (*(.fini_array)) } >ddmem = 0xff
    PROVIDE (__fini_array_end = .);

    .ctors :
    {
        /*
         * gcc uses crtbegin.o to find the start of the constructors, so we make
         * sure it is first.  Because this is a wildcard, it doesn't matter if
         * the user does not actually link against crtbegin.o; the linker won't
         * look for a file to match a wildcard.  The wildcard also means that it
         * doesn't matter which directory crtbegin.o is in.
         */

        KEEP (*crtbegin.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*(.ctors))
        . = ALIGN(4); /* REQUIRED. LD is flaky without it. */
    } >ddmem = 0xff

    .dtors :
    {
        KEEP (*crtbegin.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*(.dtors))
        . = ALIGN(4); /* REQUIRED. LD is flaky without it. */
    } >ddmem = 0xff

    .eh_frame :
    {
        KEEP (*(.eh_frame))
        . = ALIGN(4); /* REQUIRED. LD is flaky without it. */
    } >ddmem = 0xff

    .gcc_except_table :
    {
        *(.gcc_except_table .gcc_except_table.*)
        . = ALIGN(4);   /* REQUIRED. LD is flaky without it. */
    } >ddmem = 0xff

    __got_start = .;
    .got :
    {
        *(.got.plt)
        *(.got)
        *(.rel.got)
    } >ddmem = 0
    __got_end = .;

    .data ALIGN(4) : {
        __data_start = ABSOLUTE(.);
        *(.data)
        *(.data.*)
        *(.gnu.linkonce.d*)
        CONSTRUCTORS
        . = ALIGN(4);
        __data_end = ABSOLUTE(.);
    } >ddmem = 0xff

    __data_end = . ;

    .bss ALIGN(4) :
    {
        __bss_start = ABSOLUTE(.);
        __bss_start__ = ABSOLUTE(.);
        *(.dynbss)
        *(.gnu.linkonce.b*)
        *(.bss* .gnu.linkonce.b*)
        *(COMMON)
        /* Place .noinit symbols in BSS for DLDI. */
        /* This is so that they're covered by the DLDI header's reserved BSS area. */
        *(.noinit .noinit.* .gnu.linkonce.n.*)
        . = ALIGN(4); /* REQUIRED. LD is flaky without it. */
    } >ddmem

    __bss_end = .;
    __bss_end__ = .;

    _end = .;
    __end__ = .;
    PROVIDE (end = _end);

    /* DLDI header flags. */
    PROVIDE (__dldi_header_fix_flags = ((ABSOLUTE(__bss_end) - ABSOLUTE(__bss_start)) > 0 ? 8 : 0) + ((ABSOLUTE(__got_end) - ABSOLUTE(__got_start)) > 0 ? 4 : 0) + ((ABSOLUTE(__glue_end) - ABSOLUTE(__glue_start)) > 0 ? 2 : 0));
    PROVIDE (__dldi_header_driver_size = LOG2CEIL(ABSOLUTE(__bss_end) - ABSOLUTE(__text_start)));

    /* Stabs debugging sections.  */
    .stab          0 : { *(.stab) }
    .stabstr       0 : { *(.stabstr) }
    .stab.excl     0 : { *(.stab.excl) }
    .stab.exclstr  0 : { *(.stab.exclstr) }
    .stab.index    0 : { *(.stab.index) }
    .stab.indexstr 0 : { *(.stab.indexstr) }
    .comment 0 (INFO) : { *(.comment); LINKER_VERSION; }
    .gnu.build.attributes : { *(.gnu.build.attributes .gnu.build.attributes.*) }
    /* DWARF debug sections.
       Symbols in the DWARF debugging sections are relative to the beginning
       of the section so we begin them at 0.  */
    /* DWARF 1.  */
    .debug          0 : { *(.debug) }
    .line           0 : { *(.line) }
    /* GNU DWARF 1 extensions.  */
    .debug_srcinfo  0 : { *(.debug_srcinfo) }
    .debug_sfnames  0 : { *(.debug_sfnames) }
    /* DWARF 1.1 and DWARF 2.  */
    .debug_aranges  0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
    /* DWARF 2.  */
    .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev   0 : { *(.debug_abbrev) }
    .debug_line     0 : { *(.debug_line .debug_line.* .debug_line_end) }
    .debug_frame    0 : { *(.debug_frame) }
    .debug_str      0 : { *(.debug_str) }
    .debug_loc      0 : { *(.debug_loc) }
    .debug_macinfo  0 : { *(.debug_macinfo) }
    /* SGI/MIPS DWARF 2 extensions.  */
    .debug_weaknames 0 : { *(.debug_weaknames) }
    .debug_funcnames 0 : { *(.debug_funcnames) }
    .debug_typenames 0 : { *(.debug_typenames) }
    .debug_varnames  0 : { *(.debug_varnames) }
    /* DWARF 3.  */
    .debug_pubtypes 0 : { *(.debug_pubtypes) }
    .debug_ranges   0 : { *(.debug_ranges) }
    /* DWARF 5.  */
    .debug_addr     0 : { *(.debug_addr) }
    .debug_line_str 0 : { *(.debug_line_str) }
    .debug_loclists 0 : { *(.debug_loclists) }
    .debug_macro    0 : { *(.debug_macro) }
    .debug_names    0 : { *(.debug_names) }
    .debug_rnglists 0 : { *(.debug_rnglists) }
    .debug_str_offsets 0 : { *(.debug_str_offsets) }
    .debug_sup      0 : { *(.debug_sup) }
    .ARM.attributes 0 : { KEEP (*(.ARM.attributes)) KEEP (*(.gnu.attributes)) }
    .note.gnu.arm.ident 0 : { KEEP (*(.note.gnu.arm.ident)) }
    /DISCARD/ : { *(.note.GNU-stack) *(.gnu_debuglink) *(.gnu.lto_*) }
}
