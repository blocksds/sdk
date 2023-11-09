# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023

WONDERFUL_TOOLCHAIN	?= /opt/wonderful
LLVM_TEAK_PATH		?= $(WONDERFUL_TOOLCHAIN)/toolchain/llvm-teak/bin/

.PHONY: all clean

MAKE	:= make

LLVM_TEAK_CLANG_VERSION	:= $(shell $(LLVM_TEAK_PATH)clang --version 2> /dev/null)

all:
ifeq ($(strip $(LLVM_TEAK_CLANG_VERSION)),)
	@echo "Skipping DSP examples: toolchain-llvm-teak-llvm not found"
else
	@for i in `ls`; do \
		if test -e $$i/Makefile ; then \
			$(MAKE) -C $$i --no-print-directory || { exit 1;} \
		fi; \
	done;
endif

clean:
	@for i in `ls`; do \
		if test -e $$i/Makefile ; then \
			$(MAKE) -C $$i clean --no-print-directory || { exit 1;} \
		fi; \
	done;
