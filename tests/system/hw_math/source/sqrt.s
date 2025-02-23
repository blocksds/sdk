// SPDX-License-Identifier: Zlib
//
// Copyright (C) 2025 Dominik Kurz

//originally based on a routine by Wilco Dijkstra, which is public domain
#include <nds/asminc.h>

.syntax  unified
.arch    armv5te
.cpu     arm946e-s
.arm
BEGIN_ASM_FUNC isqrt_asm 
    mov    r2, #3 << 30
    mov    r1, #1 << 30
    .irp x, \
    0,1,2,3,\
    4,5,6,7,\
    8,9,10,11,\
    12,13,14,15
    cmp r0, r1, ror #2 * \x
    subhs r0, r0, r1, ror #2 * \x
    adc r1, r2, r1, lsl #1
    .endr
    bic    r0, r1, #3 << 30
    bx lr

