####################
BlocksDS usage notes
####################

1. Introduction
===============

This is just a list of tips and notes about how to use BlocksDS.

2. Section annotations in filenames
===================================

Some projects require specific functions or variables to be placed in specific
memory regions. Some functions may also need to be compiled as ARM instead of
Thumb, which is the default.

It is possible to rename your C and C++ source files to affect the whole file.
For example, a file named ``engine.arm.c`` will be built as ARM code, and a file
called ``interrupts.itcm.c`` will be placed in the ITCM memory section.

You can also use the annotations in ``<nds/ndstypes.h>``:

- ``*.dtcm.*``:  ``DTCM_DATA``, ``DTCM_BSS``
- ``*.itcm.*``: ``ITCM_CODE``
- ``*.twl.*``: ``TWL_CODE``, ``TWL_DATA``, ``TWL_BSS``
- ``*.arm.c``, ``.arm.cpp``: ``ARM_CODE``

For assembly source files, you can use the ``BEGIN_ASM_FUNC`` macro and specify
the section in the second parameter:

.. code:: asm

    #include <nds/asminc.h>

    BEGIN_ASM_FUNC my_function itcm

        mov r0, #0
        bx  lr

3. Using alternative memory layouts
===================================

ARM7 options
------------

There are three different layouts provided by BlocksDS that applications can use
for their ARM7 code. They can be selected by modifying the ARM7 makefile:

- Default: ``ds_arm7.ld`` and ``ds_arm7_crt0.o``. It expects all code to be in
  main RAM, and relocates it to IWRAM during boot.

- IWRAM: ``ds_arm7_iwram.ld`` and ``ds_arm7_iwram_crt0.o``. It expects all code
  to be in IWRAM from the start.

- VRAM: ``ds_arm7_vram.ld`` and ``ds_arm7_vram_crt0.o``. This option places all
  sections in VRAM (which needs to be configured as ARM7 memory by the ARM9) and
  expects the code to be in VRAM from the start.

ARM9 options
------------

They can be selected by changing the ``.mem`` file used in the Makefile:

- DS layout: ``ds_arm9.mem``

- DSi layout: ``dsi_arm9.mem``
