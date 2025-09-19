# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2025

BLOCKSDS	?= /opt/blocksds/core
BLOCKSDSEXT	?= /opt/blocksds/external

WONDERFUL_TOOLCHAIN	?= /opt/wonderful
ARM_NONE_EABI_PATH	?= $(WONDERFUL_TOOLCHAIN)/toolchain/gcc-arm-none-eabi/bin/

# Tools
# -----

MKDIR		:= mkdir
RM		:= rm -rf
CP		:= cp
INSTALL	:= install

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

all:
	@$(MKDIR) -p build
	@echo "  GENERATE VERSION HEADER"
	@bash generate_version_header.sh $(VERSION_STRING) \
		build/blocksds_version.h build/blocksds_version.make fixed_makefile_part

clean:
	@echo "  CLEAN"
	@$(RM) build

INSTALLDIR	?= /opt/blocksds/core/libs/libblocksds
INSTALLDIR_ABS	:= $(abspath $(INSTALLDIR))

install: all
	@echo "  INSTALL $(INSTALLDIR_ABS)"
	@test $(INSTALLDIR_ABS)
	$(V)$(RM) $(INSTALLDIR_ABS)
	$(V)$(INSTALL) -d $(INSTALLDIR_ABS) $(INSTALLDIR_ABS)/include $(INSTALLDIR_ABS)/make
	$(V)$(CP) COPYING $(INSTALLDIR_ABS)
	$(V)$(CP) build/blocksds_version.h $(INSTALLDIR_ABS)/include/
	$(V)$(CP) build/blocksds_version.make $(INSTALLDIR_ABS)/make/
