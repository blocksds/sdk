// SPDX-License-Identifier: MPL-2.0

    .syntax unified
    .arch   armv5te
    .cpu    arm946e-s

    .section ".crt0","ax"
    .global  _start

    .balign 16
    .arm

// -----------------------------------------------------------------------------
// Startup code
// -----------------------------------------------------------------------------

_start:

    mov     r0, #0x04000000 // IME = 0;
    str     r0, [r0, #0x208]

    // Set sensible stacks to allow BIOS calls

    mov     r0, #0x13       // Switch to SVC Mode
    msr     cpsr, r0
    mov     r1, #0x03000000
    sub     r1, r1, #0x1000
    mov     sp, r1
    mov     r0, #0x1F       // Switch to System Mode
    msr     cpsr, r0
    sub     r1, r1, #0x100
    mov     sp, r1

    // This sets r7 to 1 if the console is a debugger unit, 0 if it is a retail
    // unit. It also sets r8 to the end address of cached RAM for that DS model.
    // The values need to be saved until after the RAM contents have been
    // cleared so that they are saved to __dsimode and __debugger_unit.
    ldr     r3, =__libnds_mpu_setup
    blx     r3

    mov     r0, #0x12       // Switch to IRQ Mode
    msr     cpsr, r0
    ldr     sp, =__sp_irq   // Set IRQ stack

    mov     r0, #0x13       // Switch to SVC Mode
    msr     cpsr, r0
    ldr     sp, =__sp_svc   // Set SVC stack

    mov     r0, #0x1F       // Switch to System Mode
    msr     cpsr, r0
    ldr     sp, =__sp_usr   // Set user stack

    mov     r12, #0x4000000 // Read system ROM status (NTR/TWL)
    ldrb    r11, [r12, r12, lsr #12] // SCFG_A9ROM
    and     r11, r11, #0x3

    // Synchronize with ARM7

    mov     r9, #(0x0 << 8)
    str     r9, [r12, #0x180] // Send 0x0 to the ARM7

    mov     r9, #0x9
    bl      IPCSync // Wait until ARM7 sends 0x9

    mov     r9, #(0xA << 8)
    str     r9, [r12, #0x180] // Send 0xA to the ARM7

    mov     r9, #0xB
    bl      IPCSync // Wait until ARM7 sends 0xB

    mov     r9, #(0xC << 8)
    str     r9, [r12, #0x180] // Send 0xC to the ARM7

    mov     r9, #0xD
    bl      IPCSync // Wait until ARM7 sends 0xD

    mov     r9, r11, lsl #8
    str     r9, [r12, #0x180] // Send SCFG_A9ROM to the ARM7

    // Wait until ARM7 sends 0x7 to know it is ready to enter main()
    mov     r9, #0x7
    bl      IPCSync
    // Send 0x7 to the ARM7 allow it to enter main(). 0x7 is never a valid value
    // of doing (SCFG_A9ROM & 3), so we can always tell if the value in the
    // IPC_SYNC register has changed.
    mov     r9, #(0x7 << 8)
    str     r9, [r12, #0x180]

    // Copy ITCM from LMA to VMA
    ldr     r1, =__itcm_lma
    ldr     r2, =__itcm_start
    ldr     r3, =__itcm_size
    bl      CopyMem

    // Copy DTCM from LMA to VMA
    ldr     r1, =__dtcm_lma
    ldr     r2, =__dtcm_data_start
    ldr     r3, =__dtcm_size
    bl      CopyMem

    cmp     r11, #1
    ldrne   r10, =__end__       // (DS mode) heap start
    ldreq   r10, =__twl_end__   // (DSi mode) heap start
    bl      checkARGV           // Check and process argv trickery

    ldr     r0, =__bss_start__  // Clear BSS section
    ldr     r1, =__bss_size__
    bl      ClearMem

    ldr     r0, =__sbss_start   // Clear SBSS section
    ldr     r1, =__sbss_size
    bl      ClearMem

    ldr     r9, =__debugger_unit // Set DS/DSi debugger flag
    strb    r7, [r9]

    cmp     r11, #1
    bne     NotTWL

    ldr     r9, =__dsimode      // Set DSi mode flag
    strb    r11, [r9]

    // Copy TWL area (arm9i section) from LMA to VMA
    ldr     r1, =0x02ffe1c8     // Get ARM9i LMA from header
    ldr     r1, [r1]

    ldr     r2, =__arm9i_start__
    cmp     r1, r2              // Skip copy if LMA=VMA
    ldrne   r3, =__arm9i_size__
    blne    CopyMem

    ldr     r0, =__twl_bss_start__  // Clear TWL BSS section
    ldr     r1, =__twl_bss_size__
    bl      ClearMem

NotTWL:
    // Flush caches

    // The instructions above, write data to ram areas that are backed up by a cache
    // flush all the written data to RAM.
    // This is needed especially for the arm9i binary on a DSi, since it could contain
    // data sections that could be not available right at the start of the program since
    // they're still in cache
    ldr     r3, =CP15_CleanAndFlushDCache
    blx     r3

    ldr     r3, =CP15_FlushICache
    blx     r3

    // Setup heap limits

    // argv is passed to homebrew programs in an address at the end of main RAM
    // (_libnds_argv in this file, __system_argv in libnds). This is parsed,
    // turned into an array of strings, and copied to the start of the heap by
    // build_argv() in libnds.
    //
    // The heap must start after this. If a valid argv has been found, libnds
    // will have set __system_argv.endARGV to the end of the memory used by
    // argv, so we set fake_heap_start to that address.
    //
    // If no valid argv is found, the endARGV pointer is zero. In that case, the
    // heap starts at __end__ or __twl_end__.

    .equ    _libnds_argv, 0x02FFFE70 // __system_argv in libnds
    ldr     r0, =_libnds_argv

    ldr     r2, [r0, #20]   // __system_argv.endARGV
    cmp     r2, #0
    moveq   r2, r10         // r10 is __end__ or __twl_end__ (see code above)
    ldr     r1, =fake_heap_start
    str     r2, [r1]

    // The heap end is located in different places in DS consoles than in DSi
    // consoles.
    //
    // - In DS consoles the full main RAM is available from 0x2000000 to
    //   0x2400000 (or 0x2800000 in debugger consoles). There is a bit of space
    //   reserved at the end of RAM for libnds (48 KB, or 0xC000).
    //   __libnds_mpu_setup() sets r8 to the end of main RAM, so we can use the
    //   value of r8 minus the reserved space for IPC and bootstub.
    //
    // - In DSi consoles main RAM and DTCM overlap. DTCM is located before the
    //   reserved libnds RAM, so the end of the heap is actually the start of
    //   DTCM, as that will also preserve the reserved memory.

    ldr     r1, =fake_heap_end

    cmp     r11, #1 // Check if this is a DSi
    ldreq   r8, =__dtcm_start // DSi consoles
    subne   r8, r8, 0xC000    // DS consoles

    str     r8, [r1]

    // We need to do some additional initialization, but that needs to happen
    // after a valid thread context is setup. Check cothread_start() for more
    // information.

    ldr     r1, [r0,#16]    // argv
    ldr     r0, [r0,#12]    // argc
    sub     r2, sp, #1024   // main thread stack. This allocates 1 KB of stack
                            // for the scheduler and any interrupt that happens
                            // while the scheduler is running. The rest of the
                            // stack is available for the main thread.
    bic     r2, r2, #(8-1)  // Align to 8 bytes
    ldr     r3, =cothread_start     // This starts main as a thread
    ldr     lr, =__libnds_exit      // Jump to user code
    bx      r3

// -----------------------------------------------------------------------------
// Check for a commandline
// -----------------------------------------------------------------------------

checkARGV:

    ldr     r0, =_libnds_argv   // argv structure base
    mov     r1, #0
    str     r1, [r0,#12]        // clear argc
    str     r1, [r0,#16]        // clear argv

    ldr     r3, [r0]            // argv magic number
    ldr     r2, =0x5f617267     // '_arg'
    cmp     r3, r2
    strne   r1, [r0,#20]
    bxne    lr                  // Exit magic number isn't found

    ldr     r1, [r0, #4]        // Command line address
    ldr     r2, [r0, #8]        // Length of command line

    // Copy to heap
    mov     r3, r10             // Initial heap base
    str     r3, [r0, #4]        // Set command line address

    cmp     r2, #0
    subsne  r4, r3, r1          // dst - src
    bxeq    lr                  // dst == src or len == 0 : nothing to do.

    cmphi   r2, r4              // len > (dst - src)
    bhi     .copybackward

.copyforward:
    ldrb    r4, [r1], #1
    strb    r4, [r3], #1
    subs    r2, r2, #1
    bne     .copyforward
    b       .copydone

.copybackward:
    subs    r2, r2, #1
    ldrb    r4, [r1, r2]
    strb    r4, [r3, r2]
    bne     .copybackward

.copydone:
    push    {lr}
    ldr     r3, =build_argv
    blx     r3
    pop     {lr}
    bx      lr

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

// -----------------------------------------------------------------------------
// Synchronize with ARM7
// -----------------------------------------------------------------------------

IPCSync:

    ldr     r10, [r12, #0x180]
    and     r10, r10, #0xF
    cmp     r10, r9
    bne     IPCSync
    bx      lr

    .balign 4
    .pool

// -----------------------------------------------------------------------------
// Reference symbols
// -----------------------------------------------------------------------------

// The references below are never used by the code, they are symbols that the
// garbage collector of the linker should never remove.

    ldr     r0, =__secure_area__
    .pool

    .end
