@===============================================================================
@
@ Support:
@    __ndsabi_coro_resume, __ndsabi_coro_yield, __ndsabi_coro_pop
@
@ Copyright (C) 2021-2023 agbabi contributors
@ For conditions of distribution and use, see copyright notice in LICENSE.md
@
@===============================================================================

    .arm
    .align 2

    .section .text.__ndsabi_coro_resume, "ax", %progbits
    .global __ndsabi_coro_resume
    .type __ndsabi_coro_resume, %function
__ndsabi_coro_resume:
    push    {r4-r11, lr}
    mov     r1, sp

    ldr     sp, [r0]
    bic     sp, sp, #0x80000000
    pop     {r4-r11, lr}
    str     r1, [r0]

    bx      lr

    .section .text.__ndsabi_coro_yield, "ax", %progbits
    .global __ndsabi_coro_yield
    .type __ndsabi_coro_yield, %function
__ndsabi_coro_yield:
    push    {r4-r11, lr}
    mov     r2, sp

    ldr     sp, [r0]
    pop     {r4-r11, lr}
    str     r2, [r0]

    @ Move yield value into r0 and return
    mov     r0, r1
    bx      lr

    .section .text.__ndsabi_coro_pop, "ax", %progbits
    .global __ndsabi_coro_pop
    .type __ndsabi_coro_pop, %function
__ndsabi_coro_pop:
    ldr     r1, [sp, #4]

    mov     lr, pc
    bx      r1
    ldr     r1, [sp]
    @ r0 contains return value
    @ r1 points to ndsabi_coro_t*

    @ Allocate space for storing r4-r12, lr
    sub     r2, sp, #40
    ldr     r3, =__ndsabi_coro_pop
    str     r3, [r2, #36] @ Next resume will call __ndsabi_coro_pop

    @ Load suspend context
    ldr     sp, [r1]
    pop     {r4-r11, lr}

    @ Set "joined" flag
    orr     r2, r2, #0x80000000
    str     r2, [r1]

    bx      lr
