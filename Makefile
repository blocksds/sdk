# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023-2025

# Tools
# -----

CP		:= cp
INSTALL		:= install
MAKE		:= make
RM		:= rm -rf

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

# Targets
# -------

.PHONY: all clean examples install libs sys templates tests tools

all: libs sys tools

examples:
	+$(MAKE) -C examples

# libnds depends on grit and bin2c
libs: tools
	+$(MAKE) -C libs VERSION_STRING=$(VERSION_STRING)

# The default ARM7 depends on some libraries
sys: libs
	+$(MAKE) -C sys

templates:
	+$(MAKE) -C templates

tests:
	+$(MAKE) -C tests

tools:
	+$(MAKE) -C tools VERSION_STRING=$(VERSION_STRING)

# Default installation path of BlocksDS core components
INSTALLDIR	?= /opt/blocksds/core/
INSTALLDIR_ABS	:= $(abspath $(INSTALLDIR))

install: all
	@echo "  INSTALL $(INSTALLDIR_ABS)"
	@test $(INSTALLDIR_ABS)
	$(RM) $(INSTALLDIR_ABS)
	$(INSTALL) -d $(INSTALLDIR_ABS)
	+$(MAKE) -C libs install INSTALLDIR=$(INSTALLDIR_ABS)/libs
	+$(MAKE) -C sys install INSTALLDIR=$(INSTALLDIR_ABS)/sys
	+$(MAKE) -C tools install INSTALLDIR=$(INSTALLDIR_ABS)/tools
	+$(CP) -r sys/cmake $(INSTALLDIR_ABS)/cmake
	+$(CP) -r licenses $(INSTALLDIR_ABS)
	@echo $(VERSION_STRING) > $(INSTALLDIR)/version.txt

clean:
	+$(MAKE) -C examples clean
	+$(MAKE) -C libs clean
	+$(MAKE) -C sys clean
	+$(MAKE) -C tests clean
	+$(MAKE) -C templates clean
	+$(MAKE) -C tools clean
