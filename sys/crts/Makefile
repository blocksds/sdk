# SPDX-License-Identifier: Zlib
#
# Copyright (c) 2023 Antonio Niño Díaz

_MAJOR		:= 1
_MINOR		:= 1
_PATCH		:= 0

# Tools
# -----

PREFIX		:= arm-none-eabi-
CC		:= $(PREFIX)gcc
RM		:= rm -rf

# Verbose flag
# ------------

ifeq ($(VERBOSE),1)
V		:=
else
V		:= @
endif

# Targets
# -------

OBJS		:= ds_arm7_vram_crt0.o ds_arm7_crt0.o ds_arm9_crt0.o

.PHONY: all clean

all: $(OBJS)

clean:
	@echo "  CLEAN"
	$(V)$(RM) $(OBJS)

# Rules
# -----

ASFLAGS		:= -x assembler-with-cpp
ARCHARM7	:= -mcpu=arm7tdmi -mtune=arm7tdmi
ARCHARM9	:= -march=armv5te -mtune=arm946e-s

ds_arm7_vram_crt0.o: ds_arm7_crt0.s
	@echo "  AS.7    $@"
	$(V)$(CC) $(ASFLAGS) $(ARCHARM7) -DVRAM -c -o $@ $<

ds_arm7_crt0.o: ds_arm7_crt0.s
	@echo "  AS.7    $@"
	$(V)$(CC) $(ASFLAGS) $(ARCHARM7) -c -o $@ $<

ds_arm9_crt0.o: ds_arm9_crt0.s
	@echo "  AS.9    $@"
	$(V)$(CC) $(ASFLAGS) $(ARCHARM9) -c -o $@ $<
