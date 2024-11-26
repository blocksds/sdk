# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023-2024

# Tools
# -----

STRIP		:= -s
BINMODE		:= 755

HOSTCC		?= gcc
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

all: bin2c

bin2c: bin2c.c
	@echo "  HOSTCC  $<"
	$(V)$(HOSTCC) -Wall -Wextra -std=gnu11 -Wformat-truncation=0 -O3 -o $@ $<

clean:
	@echo "  CLEAN  "
	$(V)rm -rf bin2c

INSTALLDIR	?= /opt/blocksds/core/tools/bin2c
INSTALLDIR_ABS	:= $(abspath $(INSTALLDIR))

install: all
	@echo "  INSTALL $(INSTALLDIR_ABS)"
	@test $(INSTALLDIR_ABS)
	$(V)$(RM) $(INSTALLDIR_ABS)
	$(V)$(INSTALL) -d $(INSTALLDIR_ABS)
	$(V)$(INSTALL) $(STRIP) -m $(BINMODE) bin2c $(INSTALLDIR_ABS)
	$(V)$(CP) COPYING $(INSTALLDIR_ABS)
