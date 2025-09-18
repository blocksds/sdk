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

.PHONY: all clean dswifi install libblocksds libnds libteak libxm7 maxmod

all: dswifi libblocksds libnds libxm7 libteak maxmod

# Build targets
# =============

dswifi: libnds
	+$(MAKE) -C dswifi

libblocksds:
	+$(MAKE) -C libblocksds VERSION_STRING=$(VERSION_STRING)

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
	+$(MAKE) -C libblocksds install INSTALLDIR=$(INSTALLDIR_ABS)/libblocksds
	+$(MAKE) -C dswifi install INSTALLDIR=$(INSTALLDIR_ABS)/dswifi
	+$(MAKE) -C libxm7 install INSTALLDIR=$(INSTALLDIR_ABS)/libxm7
	+$(MAKE) -C maxmod install INSTALLDIR=$(INSTALLDIR_ABS)/maxmod
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
