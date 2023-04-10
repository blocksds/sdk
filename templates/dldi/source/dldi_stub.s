// SPDX-License-Identifier: Zlib
//
// Copyright (C) 2006-2016 Michael Chisholm (Chishm)
// Copyright (C) 2006-2016 Dave Murphy (WinterMute)

#include <nds/arm9/dldi_asm.h>

    .section ".crt0","ax"
    .global _start
    .align    4
    .arm

@ Driver patch file standard header -- 16 bytes

    .word   0xBF8DA5ED      @ Magic number to identify this region
    .asciz  " Chishm"       @ Identifying Magic string (8 bytes with null terminator)
    .byte   0x01            @ Version number
    .byte   DLDI_SIZE_16KB  @ Log [base-2] of the size of this driver in bytes.
    .byte   FIX_GOT | FIX_BSS | FIX_GLUE    @ Sections to fix
    .byte   0x00            @ Space allocated in the application, leave empty.

@ Text identifier - can be anything up to 47 chars + terminating null -- 48 bytes

    .align  4
    .asciz  "Default (No interface)"

@ Offsets to important sections within the data -- 32 bytes

    .align  6
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
    .word   FEATURE_MEDIUM_CANREAD | FEATURE_MEDIUM_CANWRITE | FEATURE_SLOT_NDS |
            FEATURE_ARM7_CAPABLE
#endif
    .word   startup         @ Function pointers to standard device driver functions
    .word   is_inserted
    .word   read_sectors
    .word   write_sectors
    .word   clear_status
    .word   shutdown

_start:

    .align
    .pool
    .end
