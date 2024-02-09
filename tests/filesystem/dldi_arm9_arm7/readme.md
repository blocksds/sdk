# DLDI on ARM9 or ARM7

Normally, the DLDI driver to access the SD card of the flashcard runs on the
ARM9. Most DLDI drivers have only been designed to run on the ARM9 (with
probably some exceptions like slot-2 flashcarts, like the SuperCard SD).

This demo allows you to test a DLDI driver and check if it works from ARM7 or
ARM9.

It also tests a new flag that can be added to the DLDI driver header:

```c
#define FEATURE_ARM7_CAPABLE    0x00000100
```

If this flag is detected, the DLDI driver of libnds will run on the ARM7. If it
isn't detected, it will run on the ARM9. It is possible to force either CPU by
using `dldiSetMode()`, and to get the current mode with `dldiGetMode()`.
