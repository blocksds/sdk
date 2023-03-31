// SPDX-License-Identifier: GPL-2.0+
//
// Copyright (C) 2010  Dave "WinterMute" Murphy
// Copyright (C) 2023  Antonio Niño Díaz

// This file is a reverse-engineered version of:
//
// https://github.com/devkitPro/nds-hb-menu/blob/d982d376ad8fc232c9c9b9284e26c28d0fc9d521/bootstub/bootstub.s

typedef struct {
    uint64_t signature; // "bootstub"
    uintptr_t arm9reboot;
    uintptr_t arm7reboot;
    uint32_t loader_size;
} bootstub_header_t;

// swiSoftReset() jumps to the addresses written in the locations below after
// preparing for the reset.
#define BIOS_ARM7_BOOTADDR  (*(uint32_t *)0x02FFFE34)
#define BIOS_ARM9_BOOTADDR  (*(uint32_t *)0x02FFFE24)

// the FIFO handler will detect this value, sync with the function waitcode(),
// below, then call swiSoftReset().
#define LIBNDS_RESET_CODE   0x0C04000C

// The ARM7 reset vector sets the reset address of the ARM9 to the reset vector
// of the ARM9, then it forces the ARM9 to call swiSoftReset() and jump to that
// vector. The ARM9 will then start the actual reset process, including telling
// the ARM7 to call swiSoftReset().
void hook9from7(void) // arm7reboot() -> this has to run on the ARM7
{
    // Tell the ARM9 to run this function after swiSoftReset()
    BIOS_ARM9_BOOTADDR = hook7from9;

    // Tell the ARM9 to sync with the ARM7 and call swiSoftReset()
    REG_IPC_FIFO_TX = LIBNDS_RESET_CODE;

    // ARM7 bootcode can be loaded here (37F8000h..3807DFFh)
    uint32_t *ARM7BASE = 0x037F8000;

    // Copy both the waitcode() and waitsync() code to ARM7BASE
    memcpy(ARM7BASE, waitcode, sizeof(waitcode) + sizeof(waitsync));

    ARM7BASE()

    // After returning from ARM7BASE(), the ARM7 returns to __libnds_exit() and
    // gets trapped in an infinite loop.
}

void waitcode(void)
{
    waitsync(1)

    REG_IPC_SYNC = 0x100;

    waitsync(0)

    REG_IPC_SYNC = 0x0;
}

void waitsync(uint32_t val)
{
    while ((REG_IPC_SYNC & 0x000F) != val);
}

// The ARM9 reset vector disables the cache, loads the loader code to VRAM_C,
// sets VRAM_C as the boot address of the ARM7 after a reset, and forces the
// ARM7 to call swiSoftReset() and jump to that address. The ARM9 will stay in
// an infinite loop until the ARM7 gives it a boot address. It's the
// responsibility of the loader to provide this address to the ARM9.
void hook7from9(void) // arm9reboot() -> this has to run on the ARM9
{
    REG_IME = 0;

    // ldr  r1, 0x2078              @ disable TCM and protection unit
    // mcr  p15, 0, r1, c1, c0
    //
    // @ Disable cache
    // mov  r0, #0
    // mcr  p15, 0, r0, c7, c5, 0   @ Instruction cache
    // mcr  p15, 0, r0, c7, c6, 0   @ Data cache
    // mcr  p15, 0, r0, c3, c0, 0   @ write buffer
    //
    // @ Wait for write buffer to empty
    // mcr  p15, 0, r0, c7, c10, 4

    REG_VRAMCNT_C = VRAM_ENABLE | VRAM_C_LCD;

    // This is the loader code provided right after the bootstub code. The size
    // is specified in the bootstub header. It will run in the ARM7, not the
    // ARM9. The loader code comes from here:
    //
    // https://github.com/devkitPro/nds-bootloader/tree/69cea3c5b7f3278f4b63672d345d0009b7f7d62d
    //
    // Its purpose is to reload nds-hb-menu and jump to it
    memcpy(VRAM_C, _loader, _loader_size)

    REG_VRAMCNT_C = VRAM_ENABLE | VRAM_C_ARM7_0x06000000; // Mapped to 6000000h

    REG_EXMEMCNT |= (1 << 11) | (1 << 7); // ARM7 owns both Slot-1 and Slot-2

    // Tell the ARM7 to run the code in VRAM_C after swiSoftReset()
    BIOS_ARM7_BOOTADDR = 0x06000000;

    // Tell the ARM7 to sync with the ARM9 and call swiSoftReset()
    REG_IPC_FIFO_TX = LIBNDS_RESET_CODE;

    waitsync(1)

    REG_IPC_SYNC = 0x100;

    waitsync(0)

    REG_IPC_SYNC = 0x0;

    // Set up and enter passme loop

    // Basically, we setup the value of 0x02fffe04 to be a branch to the address
    // stored in 0x02fffe24. Initially, this address is 0x02fffe04, so this is
    // an infinite loop until something writes to 0x02fffe24 and frees the ARM9.

#define ARM9_BRANCH_ADDRESS     (uint32_t *)0x02fffe04
#define ARM9_BRANCH_DESTINATION (uint32_t *)0x02fffe24

    *ARM9_BRANCH_ADDRESS = 0xE59FF018; // ldr r15, [r15 + 0x20]
    *ARM9_BRANCH_DESTINATION = ARM9_BRANCH_ADDRESS;

    ARM9_BRANCH_ADDRESS();
}
