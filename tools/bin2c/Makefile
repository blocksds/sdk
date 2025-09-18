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

# Version string handling
# -----------------------

# Try to find the SDK version if it wasn't already provided e.g. by the parent Makefile
SDK_VERSION	?= $(shell git describe --tags --exact-match --dirty 2>/dev/null)

ifneq ($(SDK_VERSION),)
    VERSION_STRING	:= "$(SDK_VERSION)"
else
    # Try to get the commit ID. --exclude to prevent any older tags from being displayed
    COMMIT_ID	:= $(shell git describe --always --dirty --exclude '*' 2>/dev/null)
    ifneq ($(COMMIT_ID),)
        VERSION_STRING	:= "commit $(COMMIT_ID)"
    else
        VERSION_STRING	:= "DEV"
    endif
endif

# Defines passed to all files
# ---------------------------

DEFINES		:= -DVERSION_STRING=\"$(VERSION_STRING)\"

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
	$(V)$(HOSTCC) $(DEFINES) -Wall -Wextra -std=gnu11 -Wformat-truncation=0 -O3 -o $@ $<

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
