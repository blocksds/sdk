/* Copyright (C) 2014-2024 Free Software Foundation, Inc.
   Copying and distribution of this script, with or without modification,
   are permitted in any medium without royalty provided the copyright
   notice and this notice are preserved.  */

/* SPDX-License-Identifier: MPL-2.0 AND FSFAP */

OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

/* User-configurable symbols. */

/* DTCM data area size. */
/* If zero, DTCM data is placed at the beginning of the DTCM block, rather than the end. */
PROVIDE(__dtcm_data_size = 0);
ASSERT((__dtcm_data_size & 3) == 0, "__dtcm_data_size must be a multiple of 4");

/* DLDI driver area size. */
PROVIDE(__dldi_size = 16384);

/* The size, in bytes, of the reserved section at the end of DTCM. */
/* Traditionally, ARM9 reserves 0x40 bytes here. */
PROVIDE(__dtcm_reserved_size = 0x40);
ASSERT((__dtcm_reserved_size & 3) == 0, "__dtcm_reserved_size must be a multiple of 4");

/* ARM supervisor (SWI calls) stack size. */
PROVIDE(__svc_stack_size = 0x100);
ASSERT((__svc_stack_size & 3) == 0, "__svc_stack_size must be a multiple of 4");

/* ARM interrupt handler stack size. */
PROVIDE(__irq_stack_size = 0x100);
ASSERT((__irq_stack_size & 3) == 0, "__irq_stack_size must be a multiple of 4");

/* ARM user stack size. Used only for stack smash validation at link time. */
PROVIDE(__usr_stack_size = 0);
ASSERT((__usr_stack_size & 3) == 0, "__usr_stack_size must be a multiple of 4");

__ewram_end	=	ORIGIN(ewram) + LENGTH(ewram);
__eheap_end	=	ORIGIN(ewram) + LENGTH(ewram);

__dtcm_start	=	ORIGIN(dtcm); /* Start of DTCM area. Must point to the beginning of DTCM memory. */
__dtcm_top	=	ORIGIN(dtcm) + LENGTH(dtcm); /* Top of DTCM area. */
__irq_flags	=	__dtcm_top - 0x08;
__irq_vector	=	__dtcm_top - 0x04;

__sp_svc	=	__dtcm_top - __dtcm_reserved_size; /* Top of SVC mode stack. */
__sp_irq	=	__sp_svc - __svc_stack_size; /* Top of IRQ mode stack. */
__sp_usr	=	__sp_irq - __irq_stack_size - __dtcm_data_size; /* Top of user stack. */

__dtcm_data_start = __dtcm_data_size > 0 ? __sp_usr : ORIGIN(dtcm); /* Start of DTCM data section. */
__dtcm_data_top = __dtcm_data_size > 0 ? __sp_irq - __irq_stack_size : __sp_usr - __usr_stack_size; /* Top of DTCM data section. */

__dldi_log2_size = LOG2CEIL(__dldi_size + 1) - 1;

PHDRS {
    main    PT_LOAD FLAGS(7);
    dtcm    PT_LOAD FLAGS(7);
    itcm    PT_LOAD FLAGS(7);
    twl     PT_LOAD FLAGS(0x100007); /* (DSi flag for ndstool | 7) */
}

SECTIONS
{
    /* Secure area reserved space */
    .secure : { *(.secure) } >ewram :main = 0

    .crt0	:
    {
        __text_start = . ;
        KEEP (*(.crt0))
        . = ALIGN(4);  /* REQUIRED. LD is flaky without it. */
    } >ewram :main = 0x00

    /* Create DLDI driver area. It should be zero-byte if DLDI code
       is not present, but set to an user-provided size if it is
           present. */
    .dldi :
    {
        __dldi_start = .;
        *(.dldi)
        . = ALIGN(4);

        __dldi_data_end = .;
        __dldi_end = __dldi_data_end > __dldi_start ? __dldi_start + __dldi_size : __dldi_start;
        . = __dldi_end;
        . = ALIGN(4);
    } >ewram :main = 0x00

    .plt : { *(.plt) } >ewram :main = 0xff

    .init :
    {
        KEEP (*(SORT_NONE(.init)))
    } >ewram :main

    .text :   /* ALIGN (4): */
    {
        *(EXCLUDE_FILE(*.itcm* *.twl*) .text)
        *(EXCLUDE_FILE(*.itcm* *.twl*) .stub)
        *(EXCLUDE_FILE(*.itcm* *.twl*) .text.*)
        /* .gnu.warning sections are handled specially by elf32.em.  */
        *(EXCLUDE_FILE(*.twl*) .gnu.warning)
        *(EXCLUDE_FILE(*.twl*) .gnu.linkonce.t*)
        *(.glue_7)
        *(.glue_7t)
        . = ALIGN(4);  /* REQUIRED. LD is flaky without it. */
    } >ewram :main = 0xff

    .fini           :
    {
        KEEP (*(.fini))
    } >ewram :main =0xff

    __text_end = . ;

    .rodata :
    {
        *(EXCLUDE_FILE(*.twl*) .rodata)
        *all.rodata*(*)
        *(EXCLUDE_FILE(*.twl*) .roda)
        *(EXCLUDE_FILE(*.twl*) .rodata.*)
        *(EXCLUDE_FILE(*.twl*) .gnu.linkonce.r*)
        SORT(CONSTRUCTORS)
        . = ALIGN(4);   /* REQUIRED. LD is flaky without it. */
    } >ewram :main = 0xff

    .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >ewram :main
     __exidx_start = .;
    ARM.exidx   : { *(.ARM.exidx* .gnu.linkonce.armexidx.*) } >ewram :main
     __exidx_end = .;

    /* Ensure the __preinit_array_start label is properly aligned.  We
       could instead move the label definition inside the section, but
       the linker would then create the section even if it turns out to
       be empty, which isn't pretty.  */
    . = ALIGN(32 / 8);
    .init_array :
    {
        PROVIDE (__preinit_array_start = .);
        PROVIDE (__bothinit_array_start = .);
        KEEP (*(.preinit_array))
        PROVIDE (__preinit_array_end = .);

        PROVIDE (__init_array_start = .);
        KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
        KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
        PROVIDE (__init_array_end = .);
        PROVIDE (__bothinit_array_end = .);
    } >ewram :main = 0xff
    .fini_array :
    {
        PROVIDE (__fini_array_start = .);
        KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
        KEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))
        /* Required by pico-exitprocs.c. */
        KEEP (*(.fini_array*))
        PROVIDE (__fini_array_end = .);
    } >ewram :main = 0xff

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
    } >ewram :main = 0xff

    .dtors :
    {
        KEEP (*crtbegin.o(.dtors))
        KEEP (*crtbegin?.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*(.dtors))
        . = ALIGN(4);   /* REQUIRED. LD is flaky without it. */
    } >ewram :main = 0xff

    .eh_frame :
    {
        KEEP (*(.eh_frame))
        . = ALIGN(4);   /* REQUIRED. LD is flaky without it. */
    } >ewram :main = 0xff

    .gcc_except_table :
    {
        *(.gcc_except_table .gcc_except_table.*)
        . = ALIGN(4);   /* REQUIRED. LD is flaky without it. */
    } >ewram :main = 0xff
    .got            : { *(.got.plt) *(.got) *(.rel.got) } >ewram :main = 0

    .ewram ALIGN(4) :
    {
        __ewram_start = ABSOLUTE(.);
        *(.ewram)
        *ewram.*(.text)
        . = ALIGN(4);   /* REQUIRED. LD is flaky without it. */
    } >ewram :main = 0xff

    .data ALIGN(4) :
    {
        __data_start = ABSOLUTE(.);
        *(EXCLUDE_FILE(*.twl*) .data)
        *(EXCLUDE_FILE(*.twl*) .data.*)
        *(EXCLUDE_FILE(*.twl*) .gnu.linkonce.d*)
        CONSTRUCTORS
        . = ALIGN(4);
    } >ewram :main = 0xff

    .tdata ALIGN(4) :
    {
        __tdata_start = ABSOLUTE(.);
        *(.tdata .tdata.* .gnu.linkonce.td.*)
        . = ALIGN(4);    /* REQUIRED. LD is flaky without it. */
        __tdata_end = ABSOLUTE(.);
        __data_end = . ;
    } >ewram :main = 0xff

    __tdata_size = __tdata_end - __tdata_start ;

    .tbss ALIGN(4) :
    {
         __tbss_start = ABSOLUTE(.);
        *(.tbss .tbss.* .gnu.linkonce.tb.*)
        *(.tcommon)
        . = ALIGN(4);    /* REQUIRED. LD is flaky without it. */
         __tbss_end = ABSOLUTE(.);
    } >ewram :main = 0

    __tbss_size = __tbss_end - __tbss_start ;

    __bss_vma = . ;

    .dtcm __dtcm_data_start :
    {
        __dtcm_lma = LOADADDR(.dtcm);
        *(.dtcm)
        *(.dtcm.*)
        . = ALIGN(4);
        __dtcm_end = ABSOLUTE(.);
    } >dtcm AT>ewram :dtcm = 0xff

    .itcm :
    {
        __itcm_lma = LOADADDR(.itcm);
        __itcm_start = ABSOLUTE(.);

        /* Vectors must be placed at the beginning of ITCM. */
        KEEP(*(.vectors .vectors.*))

        *(.itcm)
        *(.itcm.*)
        *.itcm*(.text .stub .text.*)
        . = ALIGN(4);
        __itcm_end = ABSOLUTE(.);
    } >itcm AT>ewram :itcm = 0xff

    .sbss __dtcm_end (NOLOAD): 
    {
        __sbss_start = ABSOLUTE(.);
        __sbss_start__ = ABSOLUTE(.);
        *(.sbss)
        *(.sbss.*)
        . = ALIGN(4);    /* REQUIRED. LD is flaky without it. */
        __sbss_end = ABSOLUTE(.);
    } >dtcm :NONE

    .bss __bss_vma (NOLOAD): 
    {
        __bss_start = ABSOLUTE(.);
        __bss_start__ = ABSOLUTE(.);
        *(EXCLUDE_FILE(*.twl*) .dynbss)
        *(EXCLUDE_FILE(*.twl*) .gnu.linkonce.b*)
        *(EXCLUDE_FILE(*.twl*) .bss*)
        *(EXCLUDE_FILE(*.twl*) COMMON)
        . = ALIGN(4);    /* REQUIRED. LD is flaky without it. */
        __bss_end__ = ABSOLUTE(.);
    } >ewram :NONE

    .noinit (NOLOAD):
    {
        __noinit_start = ABSOLUTE(.);
        *(EXCLUDE_FILE(*.twl*) .noinit)
        *(EXCLUDE_FILE(*.twl*) .noinit.*)
        *(EXCLUDE_FILE(*.twl*) .gnu.linkonce.n.*)
        . = ALIGN(4);    /* REQUIRED. LD is flaky without it. */
        __noinit_end = ABSOLUTE(.);
    } >ewram :NONE

    /* Space reserved for the thread local storage of main() */
    .tls ALIGN(4) (NOLOAD) :
    {
        __tls_start = ABSOLUTE(.);
        . = . + __tdata_size + __tbss_size;
        . = ALIGN(4);
         __tls_end = ABSOLUTE(.);
        __end__ = ABSOLUTE(.);
    } >ewram :NONE

    __tls_size = __tls_end - __tls_start ;

    .twl __end__ : AT(MAX(0x2400000,__end__))
    {
        __arm9i_lma__ = LOADADDR(.twl);
        __arm9i_start__ = ABSOLUTE(.);
        *(.twl)
        *(.twl.text .twl.text.*)
        *(.twl.rodata .twl.rodata.*)
        *(.twl.data .twl.data.*)
        *.twl*(.text .stub .text.* .gnu.linkonce.t.*)
        *.twl*(.rodata)
        *.twl*(.roda)
        *.twl*(.rodata.*)
        *.twl*(.data)
        *.twl*(.data.*)
        *.twl*(.gnu.linkonce.d*)
        . = ALIGN(4);
        __arm9i_end__ = ABSOLUTE(.);
    } :twl

    .twl_bss __arm9i_end__ (NOLOAD):
    {
        __twl_bss_start__ = ABSOLUTE(.);
        *(.twl_bss .twl_bss.*)
        *(.twl.bss .twl.bss.*)
        *.twl*(.dynbss)
        *.twl*(.gnu.linkonce.b*)
        *.twl*(.bss*)
        *.twl*(COMMON)
        . = ALIGN(4);
        __twl_bss_end__ = ABSOLUTE(.);
    } :NONE

    .twl_noinit __twl_bss_end__ (NOLOAD):
    {
        __twl_noinit_start__ = ABSOLUTE(.);
        *(.twl_noinit .twl_noinit.*)
        *(.twl.noinit .twl.noinit.*)
        *.twl*(.noinit)
        *.twl*(.noinit.*)
        *.twl*(.gnu.linkonce.n.*)
        . = ALIGN(4);
        __twl_noinit_end__ = ABSOLUTE(.);
        __twl_end__ = ABSOLUTE(.);
    } :NONE

    HIDDEN(__itcm_size = __itcm_end - __itcm_start);
    HIDDEN(__dtcm_size = __dtcm_end - __dtcm_data_start);
    HIDDEN(__arm9i_size__ = __arm9i_end__ - __arm9i_start__);
    HIDDEN(__bss_size__ = __bss_end__ - __bss_start__);
    HIDDEN(__sbss_size = __sbss_end - __sbss_start);
    HIDDEN(__twl_bss_size__ = __twl_bss_end__ - __twl_bss_start__);

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

ASSERT(__sbss_end <= __dtcm_data_top, "DTCM data overflow; increase __dtcm_data_size or move data out of DTCM");
