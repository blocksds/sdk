// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

#include <nds/asminc.h>
#include <nds/arm9/cp15_asm.h>

    .syntax  unified

    .text
    .arm

// r0 = pre-yield array
// r1 = post-yield array
BEGIN_ASM_FUNC set_registers_and_yield

    push    {r4-r11}

    ldmia   r0!, {r4-r11}   // Load random values
    stmia   r0!, {sp, lr}   // Preserve old sp and lr

    push    {r1, lr}
    blx     cothread_yield
    pop     {r1, lr}

    stmia   r1!, {r4-r11}
    stmia   r1!, {sp, lr}   // Save new sp and lr

    pop     {r4-r11}

    bx      lr

    .pool

// r0 = pre-yield array
// r1 = post-yield array
BEGIN_ASM_FUNC set_registers_and_yield_irq

    push    {r4-r11}

    ldmia   r0!, {r4-r11}   // Load random values
    stmia   r0!, {sp, lr}   // Preserve old sp and lr

    push    {r1, lr}
    mov     r0, #1          // IRQ_VBLANK
    blx     cothread_yield_irq
    pop     {r1, lr}

    stmia   r1!, {r4-r11}
    stmia   r1!, {sp, lr}   // Save new sp and lr

    pop     {r4-r11}

    bx      lr

    .pool
