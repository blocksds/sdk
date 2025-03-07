/* Copyright (C) 2014-2024 Free Software Foundation, Inc.
   Copying and distribution of this script, with or without modification,
   are permitted in any medium without royalty provided the copyright
   notice and this notice are preserved.  */

/* SPDX-License-Identifier: MPL-2.0 AND FSFAP */

OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

/* User-configurable symbols. */

/* The size, in bytes, of the amount of shared WRAM used by ARM7 code. */
/* Only 0 and 32768 are valid values. */
PROVIDE(__shared_wram_size = 32768);
ASSERT(__shared_wram_size == 0 || __shared_wram_size == 32768, "ARM7 shared WRAM size must be 0 KB or 32 KB");

/* The size, in bytes, of the reserved section at the end of IWRAM. */
/* Traditionally, ARM7 reserves 0x40 bytes here. */
PROVIDE(__iwram_reserved_size = 0x40);
ASSERT((__iwram_reserved_size & 3) == 0, "__iwram_reserved_size must be a multiple of 4");

/* ARM supervisor (SWI calls) stack size. */
PROVIDE(__svc_stack_size = 0x100);
ASSERT((__svc_stack_size & 3) == 0, "__svc_stack_size must be a multiple of 4");

/* ARM interrupt handler stack size. */
PROVIDE(__irq_stack_size = 0x100);
ASSERT((__irq_stack_size & 3) == 0, "__irq_stack_size must be a multiple of 4");

MEMORY {
    iwram : ORIGIN = 0x03800000 - __shared_wram_size, LENGTH = 65536 + __shared_wram_size	
    vram : ORIGIN = 0x06000000, LENGTH = 256K
}

__iwram_start	=	ORIGIN(iwram);
__iwram_top	=	ORIGIN(iwram) + LENGTH(iwram);

__sp_irq	=	__iwram_top - __iwram_reserved_size;
__sp_svc	=	__sp_irq - __irq_stack_size;
__sp_usr	=	__sp_svc - __svc_stack_size;

__irq_flags	=	0x04000000 - 8;
__irq_flagsaux	=	0x04000000 - 0x40;
__irq_vector	=	0x04000000 - 4;

SECTIONS
{
    .crt0	:
    {
        __text_start = . ;
        KEEP (*(.crt0))
        . = ALIGN(4);  /* REQUIRED. LD is flaky without it. */
    } >vram = 0xff

    __arm7_lma__ = .;
    __arm7_start__ = .;
    .init           :
    {
        KEEP (*(SORT_NONE(.init)))
    } >vram = 0xff

    .plt : { *(.plt) } >vram = 0xff

    .text :   /* ALIGN (4): */
    {
        *(.text .stub .text.* .twl .gnu.linkonce.t.*)
        KEEP (*(.text.*personality*))
        /* .gnu.warning sections are handled specially by elf32.em.  */
        *(.gnu.warning)
        *(.glue_7t) *(.glue_7) *(.v4_bx)
        . = ALIGN(4);  /* REQUIRED. LD is flaky without it. */
    } >vram = 0xff

    .fini           :
    {
        KEEP (*(.fini))
    } >vram =0xff

    __text_end = . ;

    .rodata :
    {
        *(.rodata)
        *all.rodata*(*)
        *(.roda)
        *(.rodata.*)
        *(.gnu.linkonce.r*)
        SORT(CONSTRUCTORS)
        . = ALIGN(4);   /* REQUIRED. LD is flaky without it. */
    } >vram = 0xff

    .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >vram
    __exidx_start = .;
    .ARM.exidx   : { *(.ARM.exidx* .gnu.linkonce.armexidx.*) } >vram
    __exidx_end = .;

    /* Ensure the __preinit_array_start label is properly aligned.  We
       could instead move the label definition inside the section, but
       the linker would then create the section even if it turns out to
       be empty, which isn't pretty.  */
    . = ALIGN(32 / 8);
    .preinit_array :
    {
        PROVIDE (__preinit_array_start = .);
        KEEP (*(.preinit_array))
        PROVIDE (__preinit_array_end = .);
    } >vram = 0xff
    .init_array :
    {
        PROVIDE (__init_array_start = .);
        KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
        KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
        PROVIDE (__init_array_end = .);
    } >vram = 0xff
    .fini_array :
    {
        PROVIDE (__fini_array_start = .);
        KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
        KEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))
        PROVIDE (__fini_array_end = .);
    } >vram = 0xff

    .ctors :
    {
        /* gcc uses crtbegin.o to find the start of
           the constructors, so we make sure it is
           first.  Because this is a wildcard, it
           doesn't matter if the user does not
           actually link against crtbegin.o; the
           linker won't look for a file to match a
           wildcard.  The wildcard also means that it
           doesn't matter which directory crtbegin.o
           is in.  */
        KEEP (*crtbegin.o(.ctors))
        KEEP (*crtbegin?.o(.ctors))
        /* We don't want to include the .ctor section from
           the crtend.o file until after the sorted ctors.
           The .ctor section from the crtend file contains the
           end of ctors marker and it must be last */
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*(.ctors))
        . = ALIGN(4);   /* REQUIRED. LD is flaky without it. */
    } >vram = 0xff

    .dtors :
    {
        KEEP (*crtbegin.o(.dtors))
        KEEP (*crtbegin?.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*(.dtors))
        . = ALIGN(4);   /* REQUIRED. LD is flaky without it. */
    } >vram = 0xff

    .eh_frame :
    {
        KEEP (*(.eh_frame))
        . = ALIGN(4);   /* REQUIRED. LD is flaky without it. */
    } >vram = 0xff

    .gcc_except_table :
    {
        *(.gcc_except_table .gcc_except_table.*)
        . = ALIGN(4);   /* REQUIRED. LD is flaky without it. */
    } >vram = 0xff
    .got            : { *(.got.plt) *(.got) } >vram = 0


    .data ALIGN(4) : 	{
        __data_start = ABSOLUTE(.);
        *(.data)
        *(.data.*)
        *(.gnu.linkonce.d*)
        CONSTRUCTORS
        . = ALIGN(4);
        __data_end = ABSOLUTE(.);
    } >vram = 0xff

    .tdata ALIGN(4) :
    {
        __tdata_start = ABSOLUTE(.);
        *(.tdata .tdata.* .gnu.linkonce.td.*)
        . = ALIGN(4);    /* REQUIRED. LD is flaky without it. */
        __tdata_end = ABSOLUTE(.);
        __data_end = . ;
    } >vram = 0xff

    __tdata_size = __tdata_end - __tdata_start ;

    .tbss ALIGN(4) (NOLOAD) :
    {
         __tbss_start = ABSOLUTE(.);
        *(.tbss .tbss.* .gnu.linkonce.tb.*)
        *(.tcommon)
        . = ALIGN(4);    /* REQUIRED. LD is flaky without it. */
         __tbss_end = ABSOLUTE(.);
    } >vram = 0xff

    __tbss_size = __tbss_end - __tbss_start ;

    __arm7_end__ = .;


    .bss ALIGN(4) (NOLOAD) :
    {
        __bss_start = ABSOLUTE(.);
        __bss_start__ = ABSOLUTE(.);
        *(.dynbss)
        *(.gnu.linkonce.b*)
        *(.bss*)
        *(.twl_bss)
        *(COMMON)
        . = ALIGN(4);    /* REQUIRED. LD is flaky without it. */
        __bss_end__ = ABSOLUTE(.);
    } >vram

    .noinit (NOLOAD):
    {
        __noinit_start = ABSOLUTE(.);
        *(.noinit .noinit.* .gnu.linkonce.n.*)
        . = ALIGN(4);    /* REQUIRED. LD is flaky without it. */
        __noinit_end = ABSOLUTE(.);
    } >vram

    /* Space reserved for the thread local storage of main() */
    .tls ALIGN(4) (NOLOAD) :
    {
        __tls_start = ABSOLUTE(.);
        . = . + __tdata_size + __tbss_size;
         __tls_end = ABSOLUTE(.);
        __end__ = ABSOLUTE(.);
    } >vram

    HIDDEN(__arm7_size__ = __arm7_end__ - __arm7_start__);
    HIDDEN(__bss_size__ = __bss_end__ - __bss_start__);
    
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
