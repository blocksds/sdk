# SPDX-License-Identifier: Zlib
#
# Copyright (c) 2023 Antonio Niño Díaz

# Tools
# -----

MAKE		:= make
RM		:= rm -rf

# Targets
# -------

.PHONY: all arm7 arm9 clean

all: arm9 arm7

arm9:
	@+$(MAKE) -f Makefile.arm9 --no-print-directory

arm7:
	@+$(MAKE) -f Makefile.arm7 --no-print-directory

clean:
	@echo "  CLEAN"
	@$(RM) lib build
