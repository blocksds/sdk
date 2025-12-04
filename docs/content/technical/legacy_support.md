---
title: Legacy device/launcher support
weight: 45
---

ROMs built with BlocksDS are designed first and foremost to support modern
execution environments and maximize the amount of free memory available to
the application. Unfortunately, some changes end up breaking compatibility
with legacy homebrew execution methods.

Known loaders which cause issues include:

- the PassMe/WifiMe methods,
- the MoonShell launcher,
- the Unlaunch launcher,
- many legacy cartridge devices.

Workarounds exist to run affected homebrew software on these platforms,
both from the user and developer's perspective. A detailed explanation of
the specific technical issues is also provided further below.

## User workarounds

This section is intended for users who would like to run modern homebrew
on affected legacy devices and execution methods.

### nds-hb-menu

The recommended way to launch a BlocksDS ROM on legacy devices is to use
the `nds-hb-menu` launcher, which is lightweight and appropriately
respects all modern homebrew standards.

#### Example: Slot-1 devices

- Download `nds-hb-menu` [here](https://github.com/devkitPro/nds-hb-menu/releases).
  The file that is required for this is `BOOT.NDS`. Other files in the `hbmenu`
  directory provide support for replacing some devices' launchers with `nds-hb-menu`
  directly.

- On some devices, you may need to perform a DLDI patch, otherwise the filesystem
  won't be detected. For this, you can use the `dldipatch` tool bundled with BlocksDS:

  ```bash
  /opt/blocksds/core/tools/dldipatch/dldipatch patch BOOT.NDS device_driver.dldi
  ```

#### Example: Slot-2 devices

Some Slot-2 devices require additional work for proper support.

- Download `nds-hb-menu` [here](https://github.com/devkitPro/nds-hb-menu/releases).
  The file that is required for this is `BOOT.NDS`.

- Download `ndsmall.bin` from [DarkFader's website](https://www.darkfader.net/ds/).
  A copy is also provided [here](../../ndsmall.bin).

- Download the DLDI driver. For example, the SuperCard SD should use `scsd_moon.dldi` from
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

## Developer workarounds

This section is intended for developers who would like to understand why
modern homebrew does not run on affected legacy devices and execution methods.
Workarounds are provided where available, but their use is discouraged and should
be limited to appropriate, specialized use cases.

### Missing argv support

The [argv structure](https://devkitpro.org/wiki/Homebrew_Menu) was added to libnds
[in mid-2007](https://github.com/devkitPro/libnds/commit/34f30043ce8683d48194fa009fd788da691517de);
this means that any launcher written before that date, as well as many launchers
written after that date, do not support passing command-line arguments. Most
notably, this impacts the NitroFS filesystem, which - outside of Slot-2
cartridges - relies on reading `argv[0]` to find the `.nds` file it was launched
from. As this is a missing feature rather than a bug, there is no workaround
available.

Unlaunch is a special case. It doesn't setup an `argv` struct before booting NDS
ROMs, it passes a [device list](https://problemkaputt.de/gbatek.htm#dsisdmmcdevicelist),
which is the format used by Nintendo to pass this information to official games.
BlocksDS supports this struct, so loaders that don't support `argv` but support
device lists can still use NitroFS.

Note that paths are restricted to 40 characters in the device list format.
Unlaunch uses the 8.3 file name format instead of passing full names to increase
compatibility with ROMs with long paths.

### Reduced DLDI driver reserved space

ROM files built [from 2017 onwards](https://github.com/devkitPro/libnds/commit/c9668aa8f47bd41400f485b8a9a728b517a1174d)
only reserve 16 kilobytes, as opposed to 32 kilobytes, for the DLDI driver used
to communicate with the launch device's filesystem. This frees RAM to be used
by the homebrew program. Unfortunately, some legacy launchers - most notably
MoonShell - incorrectly perform the DLDI patching process when encountering such
`.nds` files. This affects any homebrew program which links in filesystem access
code (be it FAT or NitroFS), *even if* the code is not used at runtime.

There is a workaround available to emit `.nds` files with the larger reserved DLDI
space; one has to create an assembly file (for example, `dldi_size_32k.s`) in the
source directory with the following contents:

```
.global __dldi_size
.equ __dldi_size, 32768
```

### NDS/DSi hybrid ROMs

ROM files built [from 2017 onwards](https://github.com/devkitPro/ndstool/commit/18d3c00df65a2b7a9c9f0312eaac518678869345)
are designed to simultaneously be both valid NDS and DSi-format ROMs. However,
this fully breaks some forms of executing ROMs:

- PassMe, WifiMe and Slot-2 flashcarts launch ROMs by jumping to the beginning
  of the file, normally containing the game title. On older homebrew ROMs,
  however, a branch instruction to a small built-in `.nds` loader is placed
  there.
- MoonShell, in particular, also relies on said built-in `.nds` loader to run
  homebrew applications.

By adding `-h 0x200` to the `ndstool` command line arguments, an NDS-only ROM
can be generated, which will work on the above environments. However, such
ROMs will not work at all on DSi/3DS consoles in DSi mode, so its use is
heavily discouraged.
