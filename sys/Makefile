# SPDX-License-Identifier: Zlib
#
# Copyright (c) 2023 Antonio Niño Díaz

CP		:= cp
INSTALL		:= install
RM		:= rm -rf
MAKE		:= make

.PHONY: all clean crts default_arm7 install

all: crts default_arm7

crts:
	+$(MAKE) -C crts -j`nproc`

default_arm7:
	+$(MAKE) -C default_arm7 -j`nproc`

INSTALLDIR	?= /opt/blocksds/core/sys
INSTALLDIR_ABS	:= $(abspath $(INSTALLDIR))

install: all
	@echo "  INSTALL $(INSTALLDIR_ABS)"
	@test $(INSTALLDIR_ABS)
	@$(RM) $(INSTALLDIR_ABS)
	@$(INSTALL) -d $(INSTALLDIR_ABS)
	$(CP) icon.bmp $(INSTALLDIR_ABS)
	+$(MAKE) -C crts install INSTALLDIR=$(INSTALLDIR_ABS)/crts
	+$(MAKE) -C default_arm7 install INSTALLDIR=$(INSTALLDIR_ABS)/default_arm7

clean:
	+$(MAKE) -C crts clean
	+$(MAKE) -C default_arm7 clean
