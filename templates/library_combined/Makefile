# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023

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
	@$(RM) $(VERSION_HEADER) lib build
