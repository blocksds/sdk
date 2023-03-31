#######################
Exit to loader protocol
#######################

0. Introduction
===============

Normally, the only way to exit a DS application is to switch the console off and
turn it on again. It can get annoying to do this if you're, for example, testing
many applications. ``libnds`` supports a protocol that allows an application
loader (such as the menu of a flashcard) to setup an exit routine that allows
the application to return to the loader. This is needed because the DS doesn't
have an OS, so the protocol is managed manually.

There is a bit of information about the protocol `in this link
<https://devkitpro.org/wiki/Homebrew_Menu>`_.  However, this documentation is
very lacking. There is also an open source loader that implements this protocol,
called `NDS Homebrew Menu <https://github.com/devkitPro/nds-hb-menu>`_.

This document describes the process that happens when a homebrew application
tries to return to the loader.

Before starting the application, the loader copies a small binary blob at the
end of RAM, in a location that has been predefined by the protocol. Then, the
loader starts the application.

1. The application exits
========================

The first step involves the application itself. Both the ARM9 and ARM7 CPUs can
call ``exit()``, or to return from the ``main()`` function.

``exit()`` is a standard C library function. In both devkitARM and BlocksDS all
it does is to call another function, ``__libnds_exit()``.

If the ``main()`` function returns, the effect is the same, as the ``crt0`` will
call ``__libnds_exit()`` right after ``main()`` returns.

2. ``libnds`` starts the exit to the loader
===========================================

The implementation of ``__libnds_exit()`` is in `libnds_exit.c
<https://github.com/blocksds/libnds/blob/91826293e9da6d85f77db8a631d20e6247920394/source/common/libnds_exit.c>`_.

The first thing it does is to look for the protocol structure in RAM.

This is the format of the struct that is used by the protocol:

.. code:: c

    #define BOOTSIG 0x62757473746F6F62ULL // "bootstub" in ASCII

    struct __bootstub {
        u64     bootsig;
        VoidFn  arm9reboot;
        VoidFn  arm7reboot;
        u32     bootsize;
    };

The location of this struct is 48 KiB before the end of RAM. Depending on the DS
model, the size of the RAM is different, so the location will change as well. In
``libnds`` there is a pointer called ``fake_heap_end`` that is set up during
boot and points to this location.

+-----------------+-----------+----------------------+
| Model           | RAM end   | ``bootstub`` address |
+=================+===========+======================+
| DS/DS Lite      | 0x2400000 | 0x23F4000            |
+-----------------+-----------+----------------------+
| DSi             | 0x3000000 | 0x2FF4000            |
+-----------------+-----------+----------------------+

``__libnds_exit()`` tries to access the struct at that location. If ``bootsig``
matches ``BOOTSIG``, it means that the exit to loader data has been provided and
the function can try to start the exit process. If the signature isn't found, it
will simply power off the console (which actually causes a reset to the system
menu in the DSi).

If the reset has been requested from the ARM7, ``arm7reboot()`` is called.
Similarly, if the ARM9 has requested the reset, ``arm9reboot()`` is called. Note
that in BlocksDS, ``arm7reboot()`` isn't used anymore.

The last field of the struct, ``bootsize``, is the size of the loader that
``arm7reboot()`` and ``arm9reboot()`` eventually boot into. The loader is
located right after the ``arm7reboot()`` and ``arm9reboot()`` functions. It is
simply appended to the ``bootstub`` code. This will be explained later.

3. Handshake between ARM7 and ARM9
==================================

This part of the process involves the code in `bootstub.s
<https://github.com/devkitPro/nds-hb-menu/blob/219e45a59a71eb36dc915038ec3f6908f321e6c3/bootstub/bootstub.s>`_.

This code is quite hard to read, so there is a disassembled and commented
version of that code in `bootstub.c <./bootstub.c>`_.

Exit from ARM7
--------------

This process is different in devkitPro and BlocksDS. The version of BlocksDS is
slightly simplified compared to devkitPro (it simplifies exit from the ARM7 by
reducing it to the case of exit from the ARM9). BlocksDS reserves two different
FIFO command IDs, one for each CPU, while devkitPro uses the same command ID for
both CPUs.

**BlocksDS**:

- The ARM7 sends command ``0x0C04000B`` using the FIFO registers. It corresponds
  to the following:

  .. code:: c

    FIFO_ADDRESSBIT | FIFO_IMMEDIATEBIT | FIFO_ARM7_REQUESTS_ARM9_RESET

- The ARM7 enters an infinite loop with interrupts enabled.

- The ARM9 receives the message, which causes an interrupt.

- The ARM9 FIFO interrupt handler sees this special message (``FIFO_ADDRESSBIT``
  and ``FIFO_IMMEDIATEBIT`` are never used together in normal messages) and it
  calls ``exit()``, which eventually calls ``arm9reboot()``. This means that, at
  this point, the process is the same as when the reset has been started from
  the ARM9.

**devkitPro**:

- The ARM7 calls ``arm7reboot()``.

- ``arm7reboot()`` writes the address of ``arm9reboot()`` to address
  ``BIOS_ARM9_BOOTADDR`` (``0x02FFFE24``). This is a special location known by
  the BIOS, used later.

- ``arm7reboot()`` sends command ``0x0C04000C`` to the ARM9.

- The ARM7 enters a sync routine that uses register ``REG_IPC_SYNC`` to
  synchronize both CPUs.

- The ARM9 FIFO interrupt handler sees this special message and it enters a
  similar sync procedure.

- When the sync procedure is over, the ARM7 enters an infinite loop, and the
  ARM9 calls ``swiSoftReset()``.

- ``swiSoftReset()`` makes the ARM9 jump to the address in
  ``BIOS_ARM9_BOOTADDR``. This means that, at this point, the process is the
  same as when the reset has been started from the ARM9.

Exit from ARM9
--------------

- ``arm9reboot()`` is called by the ARM9.

- It disables interrupts, the cache, TCM and the protection unit, and cleans the
  cache.

- It copies to ``VRAM_C`` the loader appended right after the ``bootstub`` code.
  It copies ``bootsize`` bytes in total. Then, it sets up ``VRAM_C`` as ARM7 RAM
  mapped to address ``0x06000000``. It's important to notice that the loader
  boot code is ARM7 code, this will be explained later.

- It switches the ownership of Slot-1 and Slot-2 to the ARM7.

- It sets the ARM7 start address (specified in ``BIOS_ARM7_BOOTADDR``, address
  ``0x02FFFE34``) as the start of ``VRAM_C`` (mapped to ``0x06000000``).

- It sends command ``0x0C04000C`` to the ARM7.

  .. code:: c

    FIFO_ADDRESSBIT | FIFO_IMMEDIATEBIT | FIFO_ARM9_REQUESTS_ARM7_RESET

- The ARM9 and the ARM7 synchronize using ``REG_IPC_SYNC``.

- The ARM9 setups a boot loop that jumps forever to the address stored in
  ``0x02FFFE24``.

- The ARM7 calls ``swiSoftReset()``, which makes it jump to the start of the
  loader in ``VRAM_C``.

- Some loader code runs in the ARM7 (read the next section).

- Eventually, the loader code will write an address to ``0x02FFFE24`` so that
  the ARM9 can end the boot loop and jump to the ARM9 code that will continue
  the process.

4. Loader of bootloader
=======================

This code runs from ``VRAM_C``, and initially it runs on the ARM7 only. The ARM7
must copy the ARM9 code somewhere outside of ``VRAM_C`` (it has been mapped to
the ARM7 so it is hidden from the ARM9). When the code of the ARM9 has been
loaded to its final destination, the ARM7 tells the ARM9 to jump there.

This loader is a small application that can use DLDI or the DSi SD driver to
load an application from the SD card, and then it boots that application.

In the case of the NDS Homebrew Menu, this loader loads the NDS ROM of the NDS
Homebrew Menu. This way, from the point of view of the user, the application
returns to the loader. In reality, the application hasn't returned to the
loader, it has just loaded the loader again!
