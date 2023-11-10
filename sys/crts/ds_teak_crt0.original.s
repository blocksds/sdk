// SPDX-License-Identifier: Zlib
//
// Copyright (C) 2020 Gericom
// Copyright (C) 2023 Antonio Niño Díaz

#include <teak/cpuregs.h>

.section .text.start

.global inttbl
inttbl:
.global _start
_start:
    br      start, always           // 0x00000
    br      trap_handler, always    // 0x00002
    br      nmi_handler, always     // 0x00004
    br      int0_handler, always    // 0x00006
    nop
    nop
    nop
    nop
    nop
    nop
    br      int1_handler, always    // 0x0000E
    nop
    nop
    nop
    nop
    nop
    nop
    br      int2_handler, always    // 0x00016
    nop
    nop
    nop
    nop
    nop
    nop

.text

.global nmi_handler
nmi_handler:
    cntx    s
    rst     MOD0_SHIFT_MODE_MASK, mod0 // Set mode arithmetic
    retic   always

.global trap_handler
trap_handler:
    cntx    s
    rst     MOD0_SHIFT_MODE_MASK, mod0 // Set mode arithmetic
    retic   always

.global trap_handler_tmr
trap_handler_tmr:
    push    stt0
    push    r0

    // Convert this trap to a proper timer IRQ
    mov     0x8204, r0  // r0 = &REG_ICU_IRQ_REQ
    set     0x400, [r0] // *r0 |= ICU_IRQ_MASK_TMR0
    rst     0x400, [r0] // *r0 &= ~ICU_IRQ_MASK_TMR0

    pop     r0
    pop     stt0
    reti    always

.global int1_handler
int1_handler:
    cntx    s
    load    0, ps01
    rst     MOD0_SHIFT_MODE_MASK, mod0 // Set mode arithmetic
    push    a0e
    pusha   a0
    push    a1e
    pusha   a1
    push    b0e
    pusha   b0
    push    b1e
    pusha   b1
    push    p0
    push    p1
    push    sv
    push    r0

    //ack     irq
    //mov     0x8202, r0  // r0 = &REG_ICU_IRQ_ACK
    //mov     0x8000, a0  // a0 = ICU_IRQ_MASK_DMA
    //mov     a0l, [r0]   // *r0 = a0l

    //call    onDMACompleted, always
    //call    ..., always

    pop     r0
    pop     sv
    pop     p1
    pop     p0
    popa    b1
    pop     b1e
    popa    b0
    pop     b0e
    popa    a1
    pop     a1e
    popa    a0
    pop     a0e
    retic   always

.global int0_handler
int0_handler:
    cntx    s
    load    0, ps01
    rst     MOD0_SHIFT_MODE_MASK, mod0 // Set mode arithmetic
    push    a0e
    pusha   a0
    push    a1e
    pusha   a1
    push    b0e
    pusha   b0
    push    b1e
    pusha   b1
    push    p0
    push    p1
    push    sv
    push    r0

    // Ack IRQ
    mov     0x8202, r0  // r0 = &REG_ICU_IRQ_ACK
    mov     0x4000, a0  // a0 = ICU_IRQ_MASK_APBP
    mov     a0l, [r0]   // *r0 = a0l

    //call    onIpcCommandReceived, always

    pop     r0
    pop     sv
    pop     p1
    pop     p0
    popa    b1
    pop     b1e
    popa    b0
    pop     b0e
    popa    a1
    pop     a1e
    popa    a0
    pop     a0e
    retic   always

.global int2_handler
int2_handler:
    cntx    s
    load    0, ps01
    rst     MOD0_SHIFT_MODE_MASK, mod0 // Set mode arithmetic
    push    a0e
    pusha   a0
    push    a1e
    pusha   a1
    push    b0e
    pusha   b0
    push    b1e
    pusha   b1
    push    p0
    push    p1
    push    sv
    push    r0

    // Ack IRQ
    mov     0x8202, r0  // r0 = &REG_ICU_IRQ_ACK
    mov     0x8000, a0  // a0 = ICU_IRQ_MASK_DMA
    mov     a0l, [r0]   // *r0 = a0l

    //call    ..., always

    pop     r0
    pop     sv
    pop     p1
    pop     p0
    popa    b1
    pop     b1e
    popa    b0
    pop     b0e
    popa    a1
    pop     a1e
    popa    a0
    pop     a0e
    retic   always

.global start
start:
    mov     (MOD3_IRQ_DISABLE | MOD3_STACK_ORDER_NORMAL), mod3
    mov     0, prpage
    nop
    nop

    mov     0, sp
    addv    0x4ff, sp
    dint
    mov     (MOD3_IRQ_DISABLE | MOD3_STACK_ORDER_NORMAL), mod3
    //eint
    call    initConfigRegs, always
    call    initConfigRegsShadow, always
    dint
    call    main, always
exit:
    br      exit, always

.global
initConfigRegs:
    rst     (MOD0_SATURATION_MASK | MOD0_SATURATION_STORE_MASK | \
             MOD0_HW_MUL_MASK | MOD0_SHIFT_MODE_MASK | \
             MOD0_PRODUCT_SHIFTER_P0_MASK | MOD0_PRODUCT_SHIFTER_P1_MASK), mod0
    set     (MOD0_SATURATION_ON | MOD0_SATURATION_STORE_ON), mod0
    load    0u8, page
    rst     (MOD1_BANKE_OPCODE | MOD1_EPI | MOD1_EPJ), mod1
    set     MOD1_MODULO_MODE_TL1, mod1
    mov     0, mod2
    mov     0, cfgi
    mov     0, cfgj
    mov     0, stepi0
    mov     0, stepj0
    mov     (AR0_PM1_POST_MODIFY_STEP_P2 | AR0_CS1_OFFSET_P1 | \
             AR0_PM0_POST_MODIFY_STEP_P0 | AR0_CS0_OFFSET_P1 | \
             AR0_RN1_REGISTER_Rn(4) | AR0_RN0_REGISTER_Rn(0)), ar0
    mov     (AR1_PM3_POST_MODIFY_STEP_PS | AR1_CS3_OFFSET_P1 | \
             AR1_PM2_POST_MODIFY_STEP_M2 | AR1_CS2_OFFSET_P1 | \
             AR1_RN3_REGISTER_Rn(5) | AR1_RN2_REGISTER_Rn(2)), ar1
    mov     (ARPn_PMIn_POST_MODIFY_STEP_P1 | ARPn_CIn_OFFSET_P0 | \
             ARPn_PMJn_POST_MODIFY_STEP_P1 | ARPn_CJn_OFFSET_P0 | \
             ARPn_RIn_REGISTER_Rn(0) | ARPn_RJn_REGISTER_Rn(4)), arp0
    mov     (ARPn_PMIn_POST_MODIFY_STEP_P2 | ARPn_CIn_OFFSET_P1 | \
             ARPn_PMJn_POST_MODIFY_STEP_P2 | ARPn_CJn_OFFSET_P1 | \
             ARPn_RIn_REGISTER_Rn(1) | ARPn_RJn_REGISTER_Rn(5)), arp1
    mov     (ARPn_PMIn_POST_MODIFY_STEP_M2 | ARPn_CIn_OFFSET_M1 | \
             ARPn_PMJn_POST_MODIFY_STEP_M2 | ARPn_CJn_OFFSET_M1 | \
             ARPn_RIn_REGISTER_Rn(2) | ARPn_RJn_REGISTER_Rn(6)), arp2
    mov     (ARPn_PMIn_POST_MODIFY_STEP_M1 | ARPn_CIn_OFFSET_P0 | \
             ARPn_PMJn_POST_MODIFY_STEP_M1 | ARPn_CJn_OFFSET_P0 | \
             ARPn_RIn_REGISTER_Rn(1) | ARPn_RJn_REGISTER_Rn(4)), arp3
    ret     always

.global initConfigRegsShadow
initConfigRegsShadow:
    cntx    s
    mov     (ST0_SATURATION_ON | ST0_IRQ_DISABLE | ST0_IRQ_INT0_DISABLE | \
             ST0_IRQ_INT1_DISABLE), st0
    mov     (ST1_DATA_MEMORY_PAGE(0) | ST1_PRODUCT_SHIFTER_P0_OFF), st1
    mov     (ST2_IRQ_INT2_DISABLE | ST2_SHIFT_MODE_ARITHMETIC), st2
    mov     (AR0_PM1_POST_MODIFY_STEP_P2 | AR0_CS1_OFFSET_P1 | \
             AR0_PM0_POST_MODIFY_STEP_P0 | AR0_CS0_OFFSET_P1 | \
             AR0_RN1_REGISTER_Rn(4) | AR0_RN0_REGISTER_Rn(0)), ar0
    mov     (AR1_PM3_POST_MODIFY_STEP_PS | AR1_CS3_OFFSET_P1 | \
             AR1_PM2_POST_MODIFY_STEP_M2 | AR1_CS2_OFFSET_P1 | \
             AR1_RN3_REGISTER_Rn(5) | AR1_RN2_REGISTER_Rn(2)), ar1
    mov     (ARPn_PMIn_POST_MODIFY_STEP_P1 | ARPn_CIn_OFFSET_P0 | \
             ARPn_PMJn_POST_MODIFY_STEP_P1 | ARPn_CJn_OFFSET_P0 | \
             ARPn_RIn_REGISTER_Rn(0) | ARPn_RJn_REGISTER_Rn(4)), arp0
    mov     (ARPn_PMIn_POST_MODIFY_STEP_P2 | ARPn_CIn_OFFSET_P1 | \
             ARPn_PMJn_POST_MODIFY_STEP_P2 | ARPn_CJn_OFFSET_P1 | \
             ARPn_RIn_REGISTER_Rn(1) | ARPn_RJn_REGISTER_Rn(5)), arp1
    mov     (ARPn_PMIn_POST_MODIFY_STEP_M2 | ARPn_CIn_OFFSET_M1 | \
             ARPn_PMJn_POST_MODIFY_STEP_M2 | ARPn_CJn_OFFSET_M1 | \
             ARPn_RIn_REGISTER_Rn(2) | ARPn_RJn_REGISTER_Rn(6)), arp2
    mov     (ARPn_PMIn_POST_MODIFY_STEP_M1 | ARPn_CIn_OFFSET_P0 | \
             ARPn_PMJn_POST_MODIFY_STEP_M1 | ARPn_CJn_OFFSET_P0 | \
             ARPn_RIn_REGISTER_Rn(1) | ARPn_RJn_REGISTER_Rn(4)), arp3
    cntx    r
    ret     always
