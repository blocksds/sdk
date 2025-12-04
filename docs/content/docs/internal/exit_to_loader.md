---
title: 'Exit to loader protocol'
weight: 4
---

## 1. Introduction

Normally, the only way to exit a DS application is to switch the console off and
turn it on again. It can get annoying to do this if you're, for example, testing
many applications. `libnds` supports a protocol that allows an application
loader (such as the menu of a flashcart) to setup an exit routine that allows
the application to return to the loader. This is needed because the DS doesn't
have an OS, so the protocol is managed manually.

There is a bit of information about the protocol
[in this link](https://devkitpro.org/wiki/Homebrew_Menu).  However, this
documentation is very lacking. There is also an open source loader that
implements this protocol, called
[NDS Homebrew Menu](https://github.com/devkitPro/nds-hb-menu).

This document describes the process that happens when a homebrew application
tries to return to the loader.

Before starting the application, the loader copies a small binary blob at the
end of RAM, in a location that has been predefined by the protocol. Then, the
loader starts the application.

Important note: The bootstub routine of the NDS Homebrew Menu (which is used by
other loaders) has a hardcoded return to a file named "BOOT.NDS" in the root of
the filesystem. In a regular DS, it will look for the file in the flashcard. In
a DSi, it will look for it in the SD card of the DSi. If you don't have a file
called "BOOT.NDS" the system won't work.

## 2. The application exits

The first step involves the application itself. Both the ARM9 and ARM7 CPUs can
call `exit()`, or to return from the `main()` function.

`exit()` is a standard C library function. In both devkitARM and BlocksDS all
it does is to call another function, `__libnds_exit()`.

If the `main()` function returns, the effect is the same, as the `crt0` will
call `__libnds_exit()` right after `main()` returns.

## 3. libnds starts the exit to the loader

The implementation of `__libnds_exit()` is in
[libnds\_exit.c](https://github.com/blocksds/libnds/blob/91826293e9da6d85f77db8a631d20e6247920394/source/common/libnds_exit.c).

The first thing it does is to look for the protocol structure in RAM.

This is the format of the struct that is used by the protocol:

```c
#define BOOTSIG 0x62757473746F6F62ULL // "bootstub" in ASCII

struct __bootstub {
    u64     bootsig;
    VoidFn  arm9reboot;
    VoidFn  arm7reboot;
    u32     bootsize;
};
```

The location of this struct is 48 KiB before the end of RAM. Depending on the DS
model, the size of the RAM is different, so the location will change as well. In
`libnds` there is a pointer called `fake_heap_end` that is set up during
boot and points to this location.

Right after the struct NDS Homebrew Menu stores a mini bootloader that attempts
to boot a ROM called "BOOT.NDS" from the root of the filesystem. There's also an
exception handler (for programs that don't set up one).

The real limit in DSi consoles is the location of the DSi NDS header,
at 0x2FFE000. In DS consoles the limit is the libnds IPC region at 0x23FF000.

This is how the NDS Homebrew Menu sets it up:

 Model           | RAM end   | bootstub + bootloader | Exception. handler | Limit
-----------------|-----------|-----------------------|--------------------|----------
 DS/DS Lite      | 0x2400000 | 0x23F4000             | 0x23FA000          | 0x23FF000
 DSi             | 0x3000000 | 0x2FF4000             | 0x2FFA000          | 0x2FFE000

So the maximum space available for the bootstub and bootloader is the one
available in DSi mode: 40 KiB.

`__libnds_exit()` tries to access the struct at the right location for the DS
model. If `bootsig` matches `BOOTSIG`, it means that the exit to loader data has
been provided and the function can try to start the exit process. If the
signature isn't found, it will simply power off the console (which actually
causes a reset to the system menu in the DSi).

If the reset has been requested from the ARM7, `arm7reboot()` is called.
Similarly, if the ARM9 has requested the reset, `arm9reboot()` is called. Note
that in BlocksDS, `arm7reboot()` isn't used anymore.

The last field of the struct, `bootsize`, is the size of the loader that
`arm7reboot()` and `arm9reboot()` eventually boot into. The loader is
located right after the `arm7reboot()` and `arm9reboot()` functions. It is
simply appended to the `bootstub` code. This will be explained later.

## 4. Handshake between ARM7 and ARM9

This part of the process involves the code in
[bootstub.s](https://github.com/devkitPro/nds-hb-menu/blob/219e45a59a71eb36dc915038ec3f6908f321e6c3/bootstub/bootstub.s).

That code is quite hard to read, so this is a disassembled and commented version
of that code:

```c
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
```

### Exit from ARM7

This process is different in devkitPro and BlocksDS. The version of BlocksDS is
slightly simplified compared to devkitPro (it simplifies exit from the ARM7 by
reducing it to the case of exit from the ARM9). BlocksDS reserves two different
FIFO command IDs, one for each CPU, while devkitPro uses the same command ID for
both CPUs.

**BlocksDS**:

- The ARM7 sends command `0x0C04000B` using the FIFO registers. It corresponds
  to the following:

  ```c
  FIFO_ADDRESSBIT | FIFO_IMMEDIATEBIT | FIFO_ARM7_REQUESTS_ARM9_RESET
  ```

- The ARM7 enters an infinite loop with interrupts enabled.

- The ARM9 receives the message, which causes an interrupt.

- The ARM9 FIFO interrupt handler sees this special message (`FIFO_ADDRESSBIT`
  and `FIFO_IMMEDIATEBIT` are never used together in normal messages) and it
  calls `exit()`, which eventually calls `arm9reboot()`. This means that, at
  this point, the process is the same as when the reset has been started from
  the ARM9.

**devkitPro**:

- The ARM7 calls `arm7reboot()`.

- `arm7reboot()` writes the address of `arm9reboot()` to address
  `BIOS_ARM9_BOOTADDR` (`0x02FFFE24`). This is a special location known by
  the BIOS, used later.

- `arm7reboot()` sends command `0x0C04000C` to the ARM9.

- The ARM7 enters a sync routine that uses register `REG_IPC_SYNC` to
  synchronize both CPUs.

- The ARM9 FIFO interrupt handler sees this special message and it enters a
  similar sync procedure.

- When the sync procedure is over, the ARM7 enters an infinite loop, and the
  ARM9 calls `swiSoftReset()`.

- `swiSoftReset()` makes the ARM9 jump to the address in
  `BIOS_ARM9_BOOTADDR`. This means that, at this point, the process is the
  same as when the reset has been started from the ARM9.

### Exit from ARM9

- `arm9reboot()` is called by the ARM9.

- It disables interrupts, the cache, TCM and the protection unit, and cleans the
  cache.

- It copies to `VRAM_C` the loader appended right after the `bootstub` code.
  It copies `bootsize` bytes in total. Then, it sets up `VRAM_C` as ARM7 RAM
  mapped to address `0x06000000`. It's important to notice that the loader
  boot code is ARM7 code, this will be explained later.

- It switches the ownership of Slot-1 and Slot-2 to the ARM7.

- It sets the ARM7 start address (specified in `BIOS_ARM7_BOOTADDR`, address
  `0x02FFFE34`) as the start of `VRAM_C` (mapped to `0x06000000`).

- It sends command `0x0C04000C` to the ARM7.

  ```c
  FIFO_ADDRESSBIT | FIFO_IMMEDIATEBIT | FIFO_ARM9_REQUESTS_ARM7_RESET
  ```

- The ARM9 and the ARM7 synchronize using `REG_IPC_SYNC`.

- The ARM9 setups a boot loop that jumps forever to the address stored in
  `0x02FFFE24`.

- The ARM7 calls `swiSoftReset()`, which makes it jump to the start of the
  loader in `VRAM_C`.

- Some loader code runs in the ARM7 (read the next section).

- Eventually, the loader code will write an address to `0x02FFFE24` so that
  the ARM9 can end the boot loop and jump to the ARM9 code that will continue
  the process.

## 5. Loader of bootloader

This code runs from `VRAM_C`, and initially it runs on the ARM7 only. The ARM7
must copy the ARM9 code somewhere outside of `VRAM_C` (it has been mapped to
the ARM7 so it is hidden from the ARM9). When the code of the ARM9 has been
loaded to its final destination, the ARM7 tells the ARM9 to jump there.

This loader is a small application that can use DLDI or the DSi SD driver to
load an application from the SD card, and then it boots that application.

In the case of the NDS Homebrew Menu, this loader loads the NDS ROM of the NDS
Homebrew Menu. This way, from the point of view of the user, the application
returns to the loader. In reality, the application hasn't returned to the
loader, it has just loaded the loader again!
