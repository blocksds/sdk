@===============================================================================
@
@ Support:
@    __ndsabi_fiq_memcpy4, __ndsabi_fiq_memcpy4x4
@
@ Copyright (C) 2021-2023 agbabi contributors
@ For conditions of distribution and use, see copyright notice in LICENSE.md
@
@===============================================================================

.include "macros.inc"

    .arm
    .align 2

    .section .text.__ndsabi_fiq_memcpy4, "ax", %progbits
    .global __ndsabi_fiq_memcpy4
    .type __ndsabi_fiq_memcpy4, %function
__ndsabi_fiq_memcpy4:
    cmp     r2, #48
    blt     .Lcopy_words

    push    {r4-r7}
    mrs     r3, cpsr

    @ Enter FIQ mode
    bic     r12, r3, #0x1f
    orr     r12, #0x11
    msr     cpsr, r12
    msr     spsr, r3

.Lloop_48:
    subs    r2, r2, #48
    ldmgeia r1!, {r3-r14}
    stmgeia r0!, {r3-r14}
    bgt     .Lloop_48

    @ Exit FIQ mode
    mrs     r3, spsr
    msr     cpsr, r3
    pop     {r4-r7}

    adds    r2, r2, #48
    bxeq    lr

.Lcopy_words:
    subs    r2, r2, #4
    ldrge   r3, [r1], #4
    strge   r3, [r0], #4
    bgt     .Lcopy_words
    bxeq    lr

    @ Copy byte & half tail
    joaobapt_test r2
    @ Copy half
    ldrcsh  r3, [r1], #2
    strcsh  r3, [r0], #2
    @ Copy byte
    ldrmib  r3, [r1]
    strmib  r3, [r0]
    bx      lr

    .section .text.__ndsabi_fiq_memcpy4x4, "ax", %progbits
    .global __ndsabi_fiq_memcpy4x4
    .type __ndsabi_fiq_memcpy4x4, %function
__ndsabi_fiq_memcpy4x4:
    push    {r4-r10}
    cmp     r2, #48
    blt     .Lcopy_tail_4x4

    @ Enter FIQ mode
    mrs     r3, cpsr
    bic     r12, r3, #0x1f
    orr     r12, #0x11
    msr     cpsr, r12
    msr     spsr, r3

.Lloop_48_4x4:
    subs    r2, r2, #48
    ldmgeia r1!, {r3-r14}
    stmgeia r0!, {r3-r14}
    bgt     .Lloop_48_4x4

    @ Exit FIQ mode
    mrs     r3, spsr
    msr     cpsr, r3

.Lcopy_tail_4x4:
    @ JoaoBapt test 48-bytes
    joaobapt_test_lsl r2, #27
    ldmcsia r1!, {r3-r10}
    stmcsia r0!, {r3-r10}
    ldmmiia r1!, {r3-r6}
    stmmiia r0!, {r3-r6}

    pop     {r4-r10}
    bx      lr
