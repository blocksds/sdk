@===============================================================================
@
@ POSIX:
@    getcontext, setcontext, swapcontext
@
@ Copyright (C) 2021-2023 agbabi contributors
@ For conditions of distribution and use, see copyright notice in LICENSE.md
@
@===============================================================================

.set OFF_MCONTEXT,  16
.set OFF_REG_R0,    OFF_MCONTEXT + 0
.set OFF_REG_R1,    OFF_MCONTEXT + 4
.set OFF_REG_R3,    OFF_MCONTEXT + 12
.set OFF_REG_SP,    OFF_MCONTEXT + 52
.set OFF_REG_LR,    OFF_MCONTEXT + 56
.set OFF_REG_PC,    OFF_MCONTEXT + 60
.set OFF_REG_CPSR,  OFF_MCONTEXT + 64

    .arm
    .align 2

    .section .text.getcontext, "ax", %progbits
    .global getcontext
    .type getcontext, %function
getcontext:
    @ Save r0
    str     r0, [r0, #OFF_REG_R0]

    @ Save r1-r12
    add     r0, r0, #OFF_REG_R1
    stmia   r0, {r1-r12, sp, lr}

    @ Load cpsr and pc
    mrs     r1, cpsr
    sub     r2, lr, #8

    @ Test lr for thumb flag
    tst     lr, #1
    orrne   r1, r1, #0x20   @ Mix with cpsr thumb flag
    addne   r2, r2, #4      @ Adjust thumb PC

    @ Save cpsr and pc
    str     r1, [r0, #(OFF_REG_CPSR - OFF_REG_R1)]
    str     r2, [r0, #(OFF_REG_PC - OFF_REG_R1)]

    @ Return 0 = success
    mov     r0, #0
.Lbx_lr:
    bx      lr

    .section .text.setcontext, "ax", %progbits
    .global setcontext
    .type setcontext, %function
setcontext:
    @ Enter target mode (IRQ disabled, ARM mode forced)
    ldr     r2, [r0, #OFF_REG_CPSR]
    orr     r1, r2, #0x80
    bic     r1, r1, #0x20
    msr     cpsr, r1

    @ Restore r3-r12
    add     r0, r0, #OFF_REG_R3
    ldmia   r0, {r3-r12}

    @ Restore sp, lr
    ldr     sp, [r0, #(OFF_REG_SP - OFF_REG_R3)]
    ldr     lr, [r0, #(OFF_REG_LR - OFF_REG_R3)]

    @ Enter undef mode (IRQ still disabled)
    msr     cpsr, #0x9b

    @ Restore cpsr into undef spsr
    msr     spsr, r2

    @ Restore pc into undef lr
    ldr     lr, [r0, #(OFF_REG_PC - OFF_REG_R3)]

    @ Restore r0-r2
    sub     r0, r0, #(OFF_REG_R3 - OFF_REG_R0)
    ldmia   r0, {r0-r2}

    @ pc = undef lr, cpsr = undef spsr
    movs    pc, lr

    .section .text.swapcontext, "ax", %progbits
    .global swapcontext
    .type swapcontext, %function
swapcontext:
    push    {r0-r1, lr}
    bl      getcontext
    pop     {r0-r1, lr}

    @ Return 0 = success
    mov     r2, #0
    str     r2, [r0, #OFF_REG_R0]

    str     sp, [r0, #OFF_REG_SP]
    str     lr, [r0, #OFF_REG_LR]
    ldr     r2, =.Lbx_lr
    str     r2, [r0, #OFF_REG_PC]

    mov     r0, r1
    b       setcontext
