---
title: 'Upgrade guide'
weight: 50
---

Newer BlocksDS versions occasionally introduce breaking changes which require
developer action, as well as non-breaking changes which developers are encouraged
to implement. These are documented here.

Please refer to the changelog for a full list of changes, including additions
and enhancements to the SDK which are not listed here.

## Upgrading to BlocksDS 1.3.0

* General changes:

  * The `ConsoleFont` struct no longer takes a `convertSingleColor`
    argument. For 4bpp uses, it is recommended to instead provide 1bpp
    font tile data, which will be automatically unpacked to use
    color indices 0 and 15. Another option is to manually ensure that
    the tile uses color indices 0 and 15 (or 0 and 255 for 8bpp
    graphics).

* ARM7-specific changes:

  * As these commands were only intended for TSC (NTR), `touchRead` and
    `touchReadTemperature` were moved to a new header, `tsc.h`, and renamed
    to `tscRead` and `tscReadTemperature` respectively.
  * In a similar fashion, `cdcTouchRead` is no longer available - it has
    been replaced by `cdcTouchReadData` which exposes all samples read;
    however, it is recommended to use `touchReadData` which supports both
    TSC (NTR) and CDC (TWL).
  * Some macro and function names have been changed:
    - `RTC_CR`, `RTC_CR8` and `HALT_CR` have been renamed to `REG_RTCCNT`,
      `REG_RTCCNT8` and `REG_HALTCNT`, respectively.
    - `SerialWaitBusy` has been renamed to `spiWaitBusy`.

## Upgrading to BlocksDS 1.2.0

* The `SOUND_FREQ` (ARM7) and `TIMER_FREQ` macros have been reworked to
  provide the nearest frequency to the one requested by the user, rounded.
  This changes their returned values in some cases; you may want to validate
  your uses of these macros.
* In `gl2d.h`, `glLoadSpriteSet` now expects an array of `uint16_t` integers
  for texture coordinates, rather than 32-bit `unsigned int`s. This has been done
  to reduce memory waste, as the coordinates always fit in a 16-bit integer.

## Upgrading to BlocksDS 1.0.0

* In `nds/bios.h`, the `PUnpackStruct` alias for `TUnpackStruct*` has been
  deprecated.
* In `nds/ndstypes.h`, the `fp` function pointer type has been deprecated.

## Upgrading to BlocksDS 0.14.0

* The `nds/arm9/grf.h` GRF file loading functions introduced in BlocksDS 0.13.0
  have been replaced.
* The `libxm7` library now uses timer 0, matching `maxmod`.

## Upgrading to BlocksDS 0.13.0

* Users are encouraged to use the new function `isHwDebugger()` over `swiIsDebugger()`
  to more accurately detect debug units.
* The GRF format used by Grit has been adjusted to properly follow the RIFF
  standard. As such, if your project relies on pre-generated GRF files, they may
  need to be regenerated.
* The Slot-2 tilt API introduced in BlocksDS 0.11.0 has been replaced.

### libnds touch screen and keyboard handling

`scanKeys()` updates the internal state of the key handling code. This is then
used by `keysHeld()` and `keysDown()`.

However, BlocksDS also requires the user to call `scanKeys()` before any of the
following functions: `touchRead()`, `keyboardGetChar()`, `keyboardUpdate()` and
the deprecated `touchReadXY()`.

This is unlikely to be a problem in most projects, as the normal thing to do is
to both scan the keys and read the touchscreen status, not just read the
touchscreen.

The reason is that the functions `scanKeys()` and `touchRead()` aren't
synchronized in any way, which creates a race condition. Consider this code:

```c
while (1)
{
    scanKeys();
    if (keysHeld() & KEY_TOUCH)
    {
        touchPosition touchPos;
        touchRead(&touchPos);
        printf("%d, %d\n", touchPos.px, touchPos.py);
    }
    swiWaitForVBlank();
}
```

The state of the X and Y buttons, as well as the state of the touch screen, is
passed from the ARM7 with a FIFO message. This message can technically happen in
between `scanKeys()` and `touchRead()`, which means that it's possible that
`keysHeld() & KEY_TOUCH` is true, but the coordinates read by `touchRead()` are
(0, 0) because the user has stopped pressing the screen right at that point, and
`scanKeys()` read the outdated values while `touchRead()` read the updated
values.

In BlocksDS, `scanKeys()` is used to latch the current state of the keys and the
touch screen. This forces the developer to call `scanKeys()`, but it also
ensures that there are no race conditions, as `scanKeys()` will read all the
state atomically.

## Upgrading to BlocksDS 0.12.0

* In `readdir()`, `.` and `..` filesystem entries are now returned, both on
  FAT and NitroFS filesystems.

## Upgrading to BlocksDS 0.11.3

* If you were using the ndstool feature of adding alternate language banner text
  (added in BlocksDS 0.6), the argument syntax has changed from `-bt5 "Text"` to
  `-bt 5 "Text"`.
