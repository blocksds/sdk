# SPDX-License-Identifier: Zlib
#
# Copyright (c) 2023 Antonio Niño Díaz

CP		:= cp
INSTALL		:= install
MAKE		:= make

.PHONY: all clean crts default_arm7 install

all: crts default_arm7

crts:
	+$(MAKE) -C crts -j`nproc`

default_arm7:
	+$(MAKE) -C default_arm7 -j`nproc`

INSTALLDIR_ABS	:= $(abspath $(INSTALLDIR))

install: all
	@echo "INSTALLDIR = '$(INSTALLDIR)' (must not be empty)"
	@test $(INSTALLDIR)
	$(INSTALL) -d $(INSTALLDIR_ABS)/sys \
		      $(INSTALLDIR_ABS)/sys/crts \
		      $(INSTALLDIR_ABS)/sys/default_arm7
	$(CP) -r crts/*.o crts/*.ld crts/*.mem $(INSTALLDIR_ABS)/sys/crts
	$(CP) -r default_arm7/arm7.elf $(INSTALLDIR_ABS)/sys/default_arm7

clean:
	+$(MAKE) -C crts clean
	+$(MAKE) -C default_arm7 clean
