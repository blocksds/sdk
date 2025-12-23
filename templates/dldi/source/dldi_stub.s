// SPDX-License-Identifier: Zlib
//
// Copyright (C) 2006-2016 Michael Chisholm (Chishm)
// Copyright (C) 2006-2016 Dave Murphy (WinterMute)

#include <nds/arm9/dldi_asm.h>

    .syntax unified
    .section ".crt0","ax"
    .global _start
    .balign 16
    .arm

@ Driver patch file standard header -- 16 bytes

    .word   0xBF8DA5ED                  @ DLDI identifier - magic number
    .asciz  " Chishm"                   @ DLDI identifier - magic string (8 bytes with null terminator)
    .byte   0x01                        @ DLDI identifier - DLDI version number
    .byte   __dldi_header_driver_size   @ Log [base-2] of the size of this driver in bytes.
                                        @ Calculated automatically in the link script.
    .byte   __dldi_header_fix_flags     @ Sections to fix.
                                        @ Calculated automatically in the link script.
    .byte   0x00                        @ Space allocated in the .nds file; leave empty.

@ Text identifier - can be anything up to 47 chars + terminating null -- 48 bytes

    .asciz  "Default (No interface)"
    .balign 64

@ Offsets to important sections within the data -- 32 bytes

    .word   __text_start    @ data start
    .word   __data_end      @ data end
    .word   __glue_start    @ Interworking glue start -- Needs address fixing
    .word   __glue_end      @ Interworking glue end
    .word   __got_start     @ GOT start               -- Needs address fixing
    .word   __got_end       @ GOT end
    .word   __bss_start     @ bss start               -- Needs setting to zero
    .word   __bss_end       @ bss end

@ IO_INTERFACE data -- 32 bytes

    .ascii  "XXXX"          @ ioType (Normally "DLDI")
#ifdef ARM9
    .word   FEATURE_MEDIUM_CANREAD | FEATURE_MEDIUM_CANWRITE | FEATURE_SLOT_NDS
#else
    .word   FEATURE_MEDIUM_CANREAD | FEATURE_MEDIUM_CANWRITE | FEATURE_SLOT_NDS | FEATURE_ARM7_CAPABLE
#endif
    .word   startup         @ Function pointers to standard device driver functions
    .word   is_inserted
    .word   read_sectors
    .word   write_sectors
    .word   clear_status
    .word   shutdown

_start:

    .balign 4
    .pool
    .end
