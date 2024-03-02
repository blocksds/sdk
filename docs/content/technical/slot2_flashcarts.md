---
title: Support for slot-2 flashcarts
weight: 70
---

ROMs built with BlocksDS support slot-2 devices. However, they need special
care. There are two options:

- You can generate NDS-only ROMs, with a bootloader injected by `ndstool`.
- You can load a regular NDS ROM with a patched `nds-hb-menu`.

## 1. Generating a NDS-only ROM with bootloader

If you add `-h 0x200` to the `ndstool` command line arguments, it will generate
a NDS-only ROM, with a bootloader that will let it boot from a slot-2 flaschart.
Essentially, the ROM is loaded to the slot-2 memory, and it remains memory
mapped during the execution of the game.

This mode will let you use nitroFS normally by reading directly from the slot-2
memory. FAT access will be unavailable.

Any ROM generated with this system will be unable of using DSi features when
loaded in a DSi, so it isn't recommended.

## 2. Load the rom through nds-hb-menu

You won't need to modify the invocation of `ndstool`, but you will need a
version of `nds-hb-menu` patched with the DLDI driver of your flashcart, and
patched to run from a slot-2 flashcard. When you load a NDS ROM through this
loader, it will patch it with the right DLDI driver. The whole ROM won't be
loaded to slot-2 memory, so nitroFS will rely on DLDI reads to load data. This
also means that FAT access is possible.

This method is recommended, as it doesn't ask the developer to modify the build
system, and no features are unavailable.

### Example: SuperCard SD

- Download `nds-hb-menu` [here](https://github.com/devkitPro/nds-hb-menu/releases).
  The file that is required for this is `BOOT.NDS`.

- Download `ndsmall.bin` from [DarkFader's website](https://www.darkfader.net/ds/).
  A copy is also provided [here](../../ndsmall.bin).

- Download `scsd_moon.dldi` from
  [this DLDI archive](https://github.com/DS-Homebrew/DLDI/blob/master/prebuilts/scsd_moon.dldi).

- Concatenate `ndsmall.bin` and `BOOT.NDS`:

  ```bash
  cat ndsmall.bin BOOT.NDS > hbmenu.sc.nds
  ```

- Patch this ROM with the DLDI driver:

  ```bash
  /opt/blocksds/core/tools/dldipatch/dldipatch patch hbmenu.sc.nds scsd_moon.dldi
  ```

- Copy `hbmenu.sc.nds` to your SD card. Any NDS homebrew ROM loaded with this
  loader should work.
