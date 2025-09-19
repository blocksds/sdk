# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023

WONDERFUL_TOOLCHAIN	?= /opt/wonderful
LLVM_TEAK_PATH		?= $(WONDERFUL_TOOLCHAIN)/toolchain/llvm-teak/bin/

LLVM_TEAK_CLANG_VERSION	:= $(shell $(LLVM_TEAK_PATH)clang --version 2> /dev/null)

# Tools
# -----

CP		:= cp
INSTALL		:= install
MAKE		:= make
RM		:= rm -rf
MKDIR		:= mkdir

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


# Build targets
# =============

.PHONY: all clean dswifi install libnds libteak libxm7 maxmod version

all: dswifi libnds libxm7 libteak maxmod version

dswifi: libnds
	+$(MAKE) -C dswifi

version:
	+$(MAKE) -C version VERSION_STRING=$(VERSION_STRING)

libnds:
	+$(MAKE) -C libnds

libteak:
ifeq ($(strip $(LLVM_TEAK_CLANG_VERSION)),)
	@echo "Skipping libteak: toolchain-llvm-teak-llvm not found"
else
	+$(MAKE) -C libteak
endif

libxm7: libnds
	+$(MAKE) -C libxm7

maxmod:
	+$(MAKE) -C maxmod ds

# Install target
# ==============

INSTALLDIR	?= /opt/blocksds/core/libs
INSTALLDIR_ABS	:= $(abspath $(INSTALLDIR))

install: all
	@echo "  INSTALL $(INSTALLDIR_ABS)"
	@test $(INSTALLDIR_ABS)
	@$(RM) $(INSTALLDIR_ABS)
	@$(INSTALL) -d $(INSTALLDIR_ABS)
	+$(MAKE) -C libnds install INSTALLDIR=$(INSTALLDIR_ABS)/libnds
	+$(MAKE) -C dswifi install INSTALLDIR=$(INSTALLDIR_ABS)/dswifi
	+$(MAKE) -C libxm7 install INSTALLDIR=$(INSTALLDIR_ABS)/libxm7
	+$(MAKE) -C maxmod install INSTALLDIR=$(INSTALLDIR_ABS)/maxmod
	+$(MAKE) -C version install INSTALLDIR=$(INSTALLDIR_ABS)/version
ifeq ($(strip $(LLVM_TEAK_CLANG_VERSION)),)
	@echo "Skipping libteak: toolchain-llvm-teak-llvm not found"
else
	+$(MAKE) -C libteak install INSTALLDIR=$(INSTALLDIR_ABS)/libteak
endif

# Clean target
# ============

clean:
	+$(MAKE) -C dswifi clean
	+$(MAKE) -C libnds clean
	+$(MAKE) -C libxm7 clean
	+$(MAKE) -C maxmod clean
ifeq ($(strip $(LLVM_TEAK_CLANG_VERSION)),)
	@echo "Skipping libteak: toolchain-llvm-teak-llvm not found"
else
	+$(MAKE) -C libteak clean
endif
