---
title: 'Memory map'
weight: 20
---

## 1. Introduction

The DS has several RAM regions that are distributed across its address space.
In most cases you don't need to worry about the details, as libnds is set up in a
way that covers most use-cases. All you normally need to worry about is how to
allocate VRAM to be used by the 2D and 3D engines. However, in some cases it may
be needed to understand how everything works.

## 2. Memory map

### 2.1 ARM9 Memory Map

Address    | Size       | Description
-----------|------------|--------------------------------------------------------
0x0000000  | 32 KB      | Instruction TCM (ITCM) (mirror-able to 1000000h)
0xxxxx000  | 16 KB      | Data TCM (DTCM) (moveable)
0x2000000  | 4 MB       | Main RAM (8 MB in debugger consoles)
0x3000000  | 0/16/32 KB | Shared WRAM
0x4000000  | -          | I/O registers
0x5000000  | 2 KB       | Standard palette RAM (for 2D BGs and sprites)
0x6000000  | -          | VRAM
0x7000000  | 2 KB       | OAM (for 2D sprites)
0x8000000  | 32 MB      | GBA slot ROM/RAM
0xA000000  | 64 KB      | GBA slot SRAM
0xFFFF0000 | 32 KB      | ARM9 BIOS

### 2.2 ARM7 Memory Map

Address   | Size       | Description
----------|------------|--------------------------------------------------------
0x0000000 | 16 KB      | ARM7 BIOS
0x2000000 | 4 MB       | Main RAM (8 MB in debugger consoles)
0x3000000 | 0/16/32 KB | Shared WRAM
0x3800000 | 64 KB      | ARM7 WRAM (64KB)
0x4000000 | -          | I/O registers
0x6000000 | -          | VRAM allocated as Work RAM to the ARM7
0x8000000 | 32 MB      | GBA slot ROM/RAM
0xA000000 | 64 KB      | GBA slot SRAM

### 2.3 DSi changes

- ARM7 BIOS is 64 KB instead of 16 KB.
- ARM9 BIOS is 64 KB instead of 32 KB.
- Main RAM is quadrupled to 16 MB (32 MB in debugger consoles).
- An additional 768 KB of WRAM is provided, referred to as NWRAM.
- The GBA slot ROM and RAM areas are filled with 0xFF.
- At 0xC000000 there is a mirror of main RAM.

## 3. Memory Protection Unit (MPU)

The ARM9 has a MPU, as well as data and instruction caches. The MPU lets you:

- Define which parts of the address space can be read/written. If you try to
  read or write an address that you're not allowed to, you will cause a data
  abort exception.
- Decide which parts of the address space can be executed as code.
- Decide which parts of the address space are cached.

The MPU doesn't let you remap any memory block. However, most memory regions are
mirrored. For example, on a regular DS, main RAM starts at 0x2000000, and the
next memory can be found at address 0x3000000. In total, this is 16 MB. However,
main RAM is only 4 MB in size. Mirroring means that the 4 MB are repeated a
total of 4 times until address 0x3000000.

Mirrors allow us to do clever things with the MPU:

- ITCM is mapped at address 0x0000000, but it is repeated until 0x2000000.
  Using it at address 0x0000000 is problematic because NULL would become a valid
  address, and normally we want it to be invalid, so that reads/writes to a NULL
  pointer cause exceptions. With the MPU we can block accesses to the first
  mirror of ITCM, and we can only let programs access it at address 0x1000000.
- Normally we want main RAM to be cached. However, it can be very useful to
  access it without cache in some cases. We can use the MPU to enable accesses
  to one of the mirrors as cached memory, and to a different mirror as uncached
  memory.

Even without an MPU, mirrors can be useful:

- The BIOS exception vector address as well as the pointer to the top of
  exception stack is stored at the
  [end of main RAM](https://github.com/blocksds/libnds/blob/43714cdc0cbcedf2a3014da3793f7b09d2fe386e/include/nds/arm9/exceptions.h#L23-L27).
  We could calculate the right address of the end of the RAM for all DS models,
  but it's easier to access it at the last mirror of main RAM right before
  0x3000000, so that it's the same address in all models.
- Shared WRAM is found at 0x3000000, but it is mirrored up to 0x3800000, where
  ARM7 WRAM starts. This means that it is possible to use the last mirror of
  shared WRAM and the first mirror of ARM7 WRAM as one contiguous memory block.

The regions of the address space that are cached in libnds are the ARM9 BIOS,
and main RAM. ITCM and DTCM aren't supposed to be cached.

If you want to know all the details of the MPU setup, you can check the
[code in libnds](https://github.com/blocksds/libnds/blob/43714cdc0cbcedf2a3014da3793f7b09d2fe386e/source/arm9/system/mpu_setup.s),
which is fairly well documented.

Note that slot 2 memory is normally uncached, but libnds provides helpers to use
RAM slot 2 cartridges, and it allows the developer to enable cache for them if
desired.

## 4. Main RAM

### 4.1. Configuration

The size and location of main RAM changes depending on the console model. All of
them have main RAM starting at address 0x2000000, this table shows the
differences:

Model        | Size     | Mirrors
-------------|----------|--------------------------------
Retail DS    | 4 MB     | 0x2400000, 0x2800000, 0x2C00000
Debugger DS  | 8 MB     | 0x2800000
Retail DSi   | 16 MB    | 0xC000000
Debugger DSi | 16+16 MB | 0xC000000

All mirrors are uncached in libnds.

On the DSi, the register `SCFG_EXT` allows the developer to set the limit of
main RAM. It allows you to map 4, 16 or 32 MB of RAM to the adddress space:

- 4 MB: Used as DS compatibility mode, the other 12 MB will be mirrors of the
  first 4 MB instead of more RAM.
- 16 MB: Normal setting.
- 32 MB: On a DSi debugger, it will expose the additional 16 MB of RAM. On a
  retail DSi, the additional 16 MB will be a mirror of the first 16 MB.

You may have realized that the debugger DSi model doesn't have enough space at
0x2000000-0x3000000 for the full 32 MB of RAM, only for 16 MB. Unfortunately,
the additional 16 MB can only be accessed at 0xD000000. This means that,
according to the setup of libnds, this area is uncached.

Debugger DSi consoles are very unusual, so they aren't a real concern. However,
3DS consoles in DSi mode can access the full 32 MB that would correspond to a
DSi debugger unit. Because of this, it is interesting to support the full 32 MB
of RAM.

### 4.2. Reserved areas

The last 48 KB of main RAM is reserved by libnds and/or contains data provided by
the .nds loader:

Address   | Description
----------|---------------------------------------------------------
0x2FF0000 | libnds shadows this area with DTCM, making it inaccessible through this address. This is only a problem in DSi mode, where it needs to be accessed from 0xCFF0000. It needs to be the first thing in the reserved memory area so that the stack can overflow into main RAM without affecting any other reserved information.
0x2FF4000 | [devkitARM bootstub structure](https://github.com/blocksds/libnds/blob/7d131d933ebab8eecf1c28a4eeb2107257f09e14/include/nds/system.h#L451-L458). Used for implementing the [exit to loader protocol](../../design/exit_to_loader/). Right after the bootstumb NDS Homebrew Menu stores a mini bootloader that attempts to boot a ROM called "BOOT.NDS" from the root of the filesystem. In total there are 0x5000 bytes reserved for the bootstub and bootloader.
0x2FFA000 | NDS Homebrew Menu copies its exception handler here. Its current size is smaller than 0x2000 bytes.
0x2FFE000 | DSi only: .nds header - 0x1000 bytes.
0x2FFF000 | libnds ARM9/ARM7 internal IPC region.
0x2FFDC00 | DSi: This is where the device list is placed by loaders normally. Only used during boot. This isn't reserved, and it is also used for the internal IPC region after boot.
0x2FFFC80 | DS/DSi: user settings loaded from flash memory.
0x2FFFD9C | DS/DSi: ARM9 exception vector, as well as top of stack.
0x2FFFE00 | DS/DSi: .nds header - 0x160 bytes on DSi, 0x170 bytes on NDS.
0x2FFFE70 | [devkitARM argv structure](https://github.com/blocksds/libnds/blob/7d131d933ebab8eecf1c28a4eeb2107257f09e14/include/nds/system.h#L435-L447). Used for implementing argument passing.

### 4.3. Caveats

Note that, if both CPUs are accessing main RAM at the same time, there will be
penalties that will delay accesses. Ideally, ARM9 and ARM7 should use different
memory regions so that this doesn't happen. That's why the ARM7 normally only
uses ARM7 WRAM and Shared WRAM (which is always mapped to the ARM7 by libnds).
This way the ARM7 has access to 96 KB of WRAM, which isn't much, but it's
normally enough.

## 5. ITCM and DTCM

Because of how fast it is, DTCM is used for the stack of the ARM9. DTCM can be
mapped anywhere, and libnds maps the 16 KB of DTCM at 0x2FF0000-0x2FF4000.
Developers may use DTCM for their own variables, but that will reduce the amount
of stack available, so be careful when doing that. Check the [usage notes](../usage_notes)
of BlocksDS to know how to optimize the DTCM layout if you want to allocate your
own variables there.

You may notice that this overlaps with 16 KB of main RAM in DSi consoles. Yes,
the main RAM that is hidden by DTCM can only be accessed through the uncached
main RAM mirror in this case.

ITCM is similarly fast, and it is used by libnds for time-sensitive code like
exception handlers. Developers may use the remaining space for anything they
like.

Something very important to consider is that only the ARM9 sees ITCM and DTCM.
The ARM7 can't see them, so you can't use them to share data between CPUs. Also,
the DMA doesn't see ITCM or DTCM, so you can't use it to copy data when the
source or destination is in ITCM or DTCM, you will have to use a regular CPU
copy.

ITCM can't be remapped, but the 32 KB of ITCM are mirrored from 0x0000000 to
0x2000000. It's a good idea to leave the region at 0x0000000 inaccessible so
that any NULL pointer that is dereferenced causes a data abort. That's why
libnds sets up the MPU so that ITCM is accessed at address 0x1000000. It
traps NULL pointer dereferences and it makes ITCM addresses more recognizable.

However, it's important to have the option for the CPU to access ITCM at address
0x0000000 because users are allowed to setup a custom exception handler there.
Access to this region is enabled if you call `setVectorBase()`.

## 6. ARM7 WRAM and Shared WRAM

This WRAM is available for the ARM7 only. It's fairly small, only 64 KB.
However, this is the main RAM that the ARM7 should use, as it will leave main
RAM to be freely accessed without any penalty by the ARM9.

Shared WRAM can be mapped to the ARM9 or ARM7 as two blocks of 16 KB. However,
because the ARM7 WRAM is so small, libnds allocates all of it to the ARM7 in a
way that the 32 KB of shared WRAM go right before the 64 KB of ARM7 WRAM, for a
total of 96 KB of WRAM.

### 6.1. Reserved areas

The last 64 bytes of ARM7 WRAM is reserved for the NDS BIOS:

Address   | Description
----------|---------------------------------------------------------
0x380FFC0 | BIOS auxillary (DSi-specific) IRQ acknowledgement
0x380FFDC | ARM7 exception vector, as well as top of stack.
0x380FFF8 | BIOS IRQ acknowledgement
0x380FFFC | BIOS IRQ handler access

### 6.2. DSi Shared WRAM (NWRAM)

The DSi introduces 3 new banks of WRAM (libnds calls it NWRAM). Each one of the
banks (A, B and C) are 256 KB in size. Each bank is divided in 32 KB slots that
can be assigned to ARM7 or ARM9. In the case of NWRAM-B and C, they can also be
assigned as memory to the DSP.

This WRAM is a bit problematic.

First, there is `SCFG_EXT`. This register can disable access to the SCFG and MBK
registers (bit 31). If the loader of the NDS ROM doesn't have enough privileges
and it loads the NDS ROM with this bit set to 0, the ROM won't be able to
reconfigure the SCFG or MBK registers.

Then, there are registers called `MBK1` up to `MBK9`, which are used to
configure NWRAM. `MBK9` can disable access to registers `MBK1` to `MBK5`, the
same registers that enable or disable the new WRAM banks.

Nintendo seems to use the convention that WRAM-A is always mapped to the ARM7,
and it's mapped to address 0x3000000. This is reliable enough that libnds uses
the same convention and always allocates WRAM-A to the ARM7 at the same address.

However, WRAM-B and C don't seem to have any specific convention (and they need
to be allocated to the DSP if the DSP is used) so there isn't any specific
default in libnds for them.

The functions that map WRAM-A/B/C in libnds always check the `MBK9` register to
ensure that it is possible to modify the settings before trying to do it.

Final note: The DSi ROM header has some fields that are used as default values
for all `MBK` registers. However, it doesn't look like all homebrew loaders use
them, so we can't rely on them.

## 7. GBA slot

In DS consoles it is possible to access the GBA slot starting at address
0x8000000. While it is possible to use this to read the ROM of GBA cartridges,
the main situation where this memory region is useful is when there is a RAM
expansion pack or some sensor or input device connected to the GBA slot.

Normally this memory region is uncached because libnds can't make any assumption
of what the GBA slot will contain, but developers can reconfigure this region as
cached (which could be useful if a RAM expansion pack is detected).

Also, note that RAM in this memory region can't be written in 8-bit units, only
in 16 or 32-bit units.

## 8. VRAM

VRAM mappings aren't very intuitive. Let's explain this with an example.

The developer configures background 2 of the main screen as a 256x256 16-bit
bitmap, using map base 0. The 2D graphics engine will try to render it by
reading from addresses 0x06000000 to 0x06020000. The developer must ensure that
there is memory mapped to that memory range. For that, the developer must setup
one of the VRAM banks (A to I) as main engine background VRAM.

For example, the developer can map VRAM A as main engine background RAM:

```c
vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
```

This will map all of the 128 KB of VRAM A starting at address 0x6000000. It is
possible to use other VRAM banks for the same purpose. Any of the following 3
lines would have the same effect:

```c
vramSetBankB(VRAM_B_MAIN_BG_0x06000000);
vramSetBankC(VRAM_C_MAIN_BG_0x06000000);
vramSetBankD(VRAM_D_MAIN_BG_0x06000000);
```

(VRAM banks E, F and G can also be used as main engine background VRAM, but they
aren't big enough to hold a 256x256 16-bit bitmap!)

If you want to allocate all VRAM A to D banks for main screen backgrounds, you
could do something like this:

```c
vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
vramSetBankB(VRAM_B_MAIN_BG_0x06020000);
vramSetBankC(VRAM_C_MAIN_BG_0x06040000);
vramSetBankD(VRAM_D_MAIN_BG_0x06060000);
```

You can write from the CPU to the addresses that you have just mapped if you
want to modify the data in VRAM.

This is the end of the explanation for backgrounds. For sprites, the explanation
is the same.

However, 3D texture data, texture palette, and extended sprite/background
palettes are different. It isn't possible for the CPU and the graphics engines
to access them at the same time. For that reason, it is needed to copy data to
them while they are in LCDC mode. This mode maps VRAM to the CPU, but the
graphics engines can't access VRAM.

For example, to load texture data and texture palettes, map the banks as LCDC:

```c
vramSetBankA(VRAM_A_LCD);
vramSetBankB(VRAM_B_LCD);
vramSetBankE(VRAM_E_LCD);
```

This will make VRAM A and B available starting at address 0x6800000, and VRAM E
available starting at address 0x6880000. After the data has been copied to VRAM
you can set them up as texture data and texture palette RAM:

```c
vramSetBankA(VRAM_A_TEXTURE_SLOT0);
vramSetBankB(VRAM_B_TEXTURE_SLOT1);
vramSetBankE(VRAM_E_TEX_PALETTE);
```

The addresses where VRAM can be mapped are:

Base address | Which engine | Usage
-------------|--------------|----------------------------
0x6000000    | Main engine  | BG VRAM (max 512KB)
0x6200000    | Sub engine   | BG VRAM (max 128KB)
0x6400000    | Main engine  | Sprite VRAM (max 256KB)
0x6600000    | Sub engine   | Sprite VRAM (max 128KB)
0x6800000    | LCDC         | LCDC-allocated (max 656KB)

LCDC mode is also special in another way. The main graphics engine can capture
its output and save it to one of the main VRAM banks (A to D). For that to work,
the bank needs to be setup as LCDC.

It is also possible to setup one of the main VRAM banks as LCDC and ask the main
graphics engine to render its contents directly to the screen.  This is
equivalent to using that VRAM bank as a framebuffer.

If you want to configure VRAM in a visual way, you should use mtheall's
[NDS Homebrew VRAM Banks Selector](https://mtheall.com/banks.html) and
[NDS Homebrew VRAM BG Allocation Conflict Viewer](https://mtheall.com/vram.html).
