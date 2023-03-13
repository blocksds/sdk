# SPDX-License-Identifier: Zlib
#
# Copyright (c) 2023 Antonio Niño Díaz

# Tools
# -----

MAKE		:= make
RM		:= rm -rf
CP		:= cp
MKDIR		:= mkdir

# Targets
# -------

.PHONY: all arm7 arm9 clean install

all: arm9 arm7

arm9: lib/libstdc++9.a

arm7: lib/libstdc++7.a

lib/libstdc++9.a:
	@+$(MAKE) -f Makefile.arm9 --no-print-directory

lib/libstdc++7.a:
	@+$(MAKE) -f Makefile.arm7 --no-print-directory

clean:
	@echo "  CLEAN"
	@$(RM) lib build

install: arm7 arm9
	$(RM) ../libstdc++7 ../libstdc++9
	$(MKDIR) -p ../libstdc++7/lib ../libstdc++9/lib
	$(CP) -r avr-libstdcpp/include ../libstdc++7
	$(CP) -r avr-libstdcpp/include ../libstdc++9
	$(CP) lib/libstdc++7.a ../libstdc++7/lib/libstdc++.a
	$(CP) lib/libstdc++9.a ../libstdc++9/lib/libstdc++.a
