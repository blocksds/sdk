# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023-2026

WONDERFUL_TOOLCHAIN	?= /opt/wonderful
XTENSA_ELF_PATH		?= $(WONDERFUL_TOOLCHAIN)/toolchain/gcc-xtensa-elf/bin/

.PHONY: all clean

MAKE	:= make

XTENSA_ELF_GCC_VERSION	:= $(shell $(XTENSA_ELF_PATH)xtensa-elf-gcc --version 2> /dev/null)

all:
ifeq ($(strip $(XTENSA_ELF_GCC_VERSION)),)
	@echo "Skipping Atheros examples: toolchain-gcc-xtensa-elf not found"
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
