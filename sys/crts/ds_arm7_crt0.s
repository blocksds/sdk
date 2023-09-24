// SPDX-License-Identifier: MPL-2.0

    .section ".crt0","ax"
    .global  _start

    .align 4
    .arm

// -----------------------------------------------------------------------------
// Startup code
// -----------------------------------------------------------------------------

_start:

    mov     r0, #0x04000000 // IME = 0
    mov     r1, #0
    str     r1, [r0, #0x208]

    mov     r0, #0x12       // Switch to IRQ Mode
    msr     cpsr, r0
    ldr     sp, =__sp_irq   // Set IRQ stack

    mov     r0, #0x13       // Switch to SVC Mode
    msr     cpsr, r0
    ldr     sp, =__sp_svc   // Set SVC stack

    mov     r0, #0x1F       // Switch to System Mode
    msr     cpsr, r0
    ldr     sp, =__sp_usr   // Set system and user stack

#ifndef VRAM

    adr     r1, __sync_start    // Perform ARM7<->ARM9 sync code
    ldr     r2, =__arm7_start__
    mov     r3, #(__sync_end - __sync_start)
    mov     r8, r2
    bl      CopyMem
    mov     r3, r8
    bl      _blx_r3_stub

    // Copy arm7 binary from LMA to VMA (EWRAM to IWRAM)

    adr     r0, arm7lma         // Calculate ARM7 LMA
    ldr     r1, [r0]
    add     r1, r1, r0
    ldr     r2, =__arm7_start__
    ldr     r4, =__arm7_end__
    bl      CopyMemCheck

#else

    bl      __sync_start

#endif

    ldr     r0, =__bss_start__  // Clear BSS section to 0x00
    ldr     r1, =__bss_end__
    sub     r1, r1, r0
    bl      ClearMem

    cmp     r10, #1             // r10 contains SCFG_A9ROM
    bne     NotTWL
    ldr     r1, =__dsimode      // Set DSi mode flag
    strb    r10, [r1]

#ifndef VRAM

    ldr     r1, =0x02ffe1d8     // Get ARM7i LMA from header
    ldr     r1, [r1]
    ldr     r2, =__arm7i_start__
    ldr     r4, =__arm7i_end__
    bl      CopyMemCheck

    ldr     r0, =__twl_bss_start__  // Clear TWL BSS section to 0x00
    ldr     r1, =__twl_bss_end__
    sub     r1, r1, r0
    bl      ClearMem

#endif

NotTWL:

    // Initialize TLS of the main thread
    ldr     r3, =__tls_start
    bl      init_tls

    ldr     r3, =__libc_init_array  // global constructors
    bl      _blx_r3_stub

    // Prepare address, arguments and return address of main().
    mov     r0, #0              // int argc
    mov     r1, #0              // char *argv[]
    ldr     r3, =main
    ldr     lr, =__libnds_exit

    // Send 0x0 to the ARM9 to tell it that we are ready
    mov     r12, #0x4000000
    mov     r9, #0
    str     r9, [r12, #0x180]

    // Fall through to call main()

_blx_r3_stub:
    bx      r3

#ifndef VRAM
arm7lma:
    .word   __arm7_lma__ - .
#endif
    .pool

// -----------------------------------------------------------------------------
// ARM7 <-> ARM9 synchronization code
// -----------------------------------------------------------------------------

// This function returns SCFG_A9ROM in r10
__sync_start:
    push    {lr}
    mov     r12, #0x4000000

    mov     r9, #0x0
    bl      IPCSync // Wait until ARM9 sends 0x0

    mov     r9, #(0x9 << 8)
    str     r9, [r12, #0x180] // Send 0x9 to the ARM9

    mov     r9, #0xA
    bl      IPCSync // Wait until ARM9 sends 0xA

    mov     r9, #(0xB << 8)
    str     r9, [r12, #0x180] // Send 0xB to the ARM9

    mov     r9, #0xC
    bl      IPCSync // Wait until ARM9 sends 0xC

    mov     r9, #(0xD << 8)
    str     r9, [r12, #0x180] // Send 0xD to the ARM9

    // Wait until the ARM9 changes the previously sent 0xC. In place of that
    // 0xC, it will send the result of reading SCFG_A9ROM
IPCRecvFlag:
    ldr     r10, [r12, #0x180]
    and     r10, r10, #0xF
    cmp     r10, #0xC
    beq     IPCRecvFlag
    pop     {pc}

IPCSync:
    ldr     r10, [r12, #0x180]
    and     r10, r10, #0xF
    cmp     r10, r9
    bne     IPCSync
    bx      lr
__sync_end:

// -----------------------------------------------------------------------------
// Clear memory to 0x00
//  r0 = Start Address
//  r1 = Length (if zero, it returns right away)
// -----------------------------------------------------------------------------

ClearMem:

    mov     r2, #3      // Round down to nearest word boundary
    add     r1, r1, r2  // Shouldn't be needed
    bics    r1, r1, r2  // Clear 2 LSB (and set Z)
    bxeq    lr          // Quit if copy size is 0

    mov     r2, #0
ClrLoop:
    stmia   r0!, {r2}
    subs    r1, r1, #4
    bne     ClrLoop
    bx      lr

// -----------------------------------------------------------------------------
// Copy memory if length
//  r1 = Source Address
//  r2 = Dest Address
//  r4 = Dest Address + Length (if length is zero, it returns right away)
// -----------------------------------------------------------------------------

CopyMemCheck:

    cmp     r1, r2
    bxeq    lr

    sub     r3, r4, r2  // Is there any data to copy?

    // Fallthrough

// -----------------------------------------------------------------------------
// Copy memory
//  r1 = Source Address
//  r2 = Dest Address
//  r3 = Length
// -----------------------------------------------------------------------------

CopyMem:

    mov     r0, #3          // These commands are used in cases where
    add     r3, r3, r0      // the length is not a multiple of 4,
    bics    r3, r3, r0      // even though it should be.
    bxeq    lr              // Length is zero, so exit
CIDLoop:
    ldmia   r1!, {r0}
    stmia   r2!, {r0}
    subs    r3, r3, #4
    bne     CIDLoop
    bx      lr

    .align
    .pool
    .end
