# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023

CP		:= cp
INSTALL		:= install
RM		:= rm -rf
MAKE		:= make

.PHONY: all clean main_core install

all: main_core

main_core:
	$(MAKE) -C main_core

INSTALLDIR	?= /opt/blocksds/core/sys/arm7
INSTALLDIR_ABS	:= $(abspath $(INSTALLDIR))

install: all
	@echo "  INSTALL $(INSTALLDIR_ABS)"
	@test $(INSTALLDIR_ABS)
	@$(RM) $(INSTALLDIR_ABS)
	@$(INSTALL) -d $(INSTALLDIR_ABS)
	$(MAKE) -C main_core install INSTALLDIR=$(INSTALLDIR_ABS)/main_core

clean:
	$(MAKE) -C main_core clean
