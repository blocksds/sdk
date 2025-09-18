# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023-2025

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

# Try to generate a version string if it isn't already provided
ifeq ($(VERSION_STRING),)
    # Try an exact match with a tag (e.g. v1.12.1)
    VERSION_STRING	:= $(shell git describe --tags --exact-match --dirty 2>/dev/null)
    ifeq ($(VERSION_STRING),)
        # Try a non-exact match (e.g. v1.12.1-3-g67a811a)
        VERSION_STRING	:= $(shell git describe --tags --dirty 2>/dev/null)
        ifeq ($(VERSION_STRING),)
            # If no version is provided by the user or git, fall back to this
            VERSION_STRING	:= DEV
        endif
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
