# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023

# Tools
# -----

STRIP		:= -s
BINMODE		:= 755

CC		:= gcc
CP		:= cp
RM		:= rm -rf
INSTALL		:= install

# Verbose flag
# ------------

ifeq ($(VERBOSE),1)
V		:=
else
V		:= @
endif

# Targets
# -------

.PHONY: all clean install

all: dlditool

dlditool: dlditool.c
	$(V)$(CC) -Wall -Wextra -std=gnu11 -O3 -o $@ $<

clean:
	rm -rf dlditool

INSTALLDIR	?= /opt/blocksds/core/tools/dlditool
INSTALLDIR_ABS	:= $(abspath $(INSTALLDIR))

install: all
	@echo "  INSTALL $(INSTALLDIR_ABS)"
	@test $(INSTALLDIR_ABS)
	$(V)$(RM) $(INSTALLDIR_ABS)
	$(V)$(INSTALL) -d $(INSTALLDIR_ABS)
	$(V)$(INSTALL) $(STRIP) -m $(BINMODE) dlditool $(INSTALLDIR_ABS)
	$(V)$(CP) COPYING $(INSTALLDIR_ABS)
