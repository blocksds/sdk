# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023

BLOCKSDS	?= /opt/blocksds/core
BLOCKSDSEXT	?= /opt/blocksds/external

WONDERFUL_TOOLCHAIN	?= /opt/wonderful
ARM_NONE_EABI_PATH	?= $(WONDERFUL_TOOLCHAIN)/toolchain/gcc-arm-none-eabi/bin/

# Libraries
# ---------

# Only headers can be used
LIBDIRS		+= $(BLOCKSDS)/libs/libnds

# Tools
# -----

CP		:= cp
INSTALL		:= install
CC		:= $(ARM_NONE_EABI_PATH)arm-none-eabi-gcc
RM		:= rm -rf

# Verbose flag
# ------------

ifeq ($(VERBOSE),1)
V		:=
else
V		:= @
endif

# Compiler and linker flags
# -------------------------

INCLUDEFLAGS	:= $(foreach path,$(LIBDIRS),-I$(path)/include)

ASFLAGS		:= -x assembler-with-cpp
ARCHARM7	:= -mcpu=arm7tdmi -mtune=arm7tdmi
ARCHARM9	:= -march=armv5te -mtune=arm946e-s

DEFINESARM7	:= -D__NDS__ -DARM7
DEFINESARM9	:= -D__NDS__ -DARM9

# Targets
# -------

OBJS		:= ds_arm7_vram_crt0.o ds_arm7_crt0.o ds_arm7_iwram_crt0.o \
			   ds_arm9_crt0.o

.PHONY: all clean install

all: $(OBJS)

clean:
	@echo "  CLEAN"
	$(V)$(RM) $(OBJS)

INSTALLDIR	?= /opt/blocksds/core/sys/crts
INSTALLDIR_ABS	:= $(abspath $(INSTALLDIR))

install: all
	@echo "  INSTALL $(INSTALLDIR_ABS)"
	@test $(INSTALLDIR_ABS)
	$(V)$(RM) $(INSTALLDIR_ABS)
	$(V)$(INSTALL) -d $(INSTALLDIR_ABS)
	$(V)$(CP) -r ./*.o ./*.ld ./*.mem ./COPYING* $(INSTALLDIR_ABS)

# Rules
# -----

ds_arm7_vram_crt0.o: ds_arm7_crt0.s
	@echo "  AS.7    $@"
	$(V)$(CC) $(ASFLAGS) $(ARCHARM7) $(INCLUDEFLAGS) $(DEFINESARM7) -DVRAM -c -o $@ $<

# They are the same, but it's more intuitive to have two files than to remember
# that the VRAM crt0 works with the IWRAM linkerscript.
ds_arm7_iwram_crt0.o: ds_arm7_vram_crt0.o
	@echo "  CP      $@"
	$(V)$(CP) $< $@

ds_arm7_crt0.o: ds_arm7_crt0.s
	@echo "  AS.7    $@"
	$(V)$(CC) $(ASFLAGS) $(ARCHARM7) $(INCLUDEFLAGS) $(DEFINESARM7) -c -o $@ $<

ds_arm9_crt0.o: ds_arm9_crt0.s
	@echo "  AS.9    $@"
	$(V)$(CC) $(ASFLAGS) $(ARCHARM9) $(INCLUDEFLAGS) $(DEFINESARM9) -c -o $@ $<
