# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023


# Tools
# -----

MAKE		:= make
INSTALL		:= install
CP		:= cp
RM		:= rm -rf

# Verbose flag
# ------------

ifeq ($(VERBOSE),1)
V		:=
else
V		:= @
endif

# Targets
# -------

.PHONY: all clean install minimal dswifi maxmod libxm7 dswifi_maxmod dswifi_libxm7

all: minimal dswifi maxmod libxm7 dswifi_maxmod dswifi_libxm7

clean:
	@echo "  CLEAN"
	@rm -rf build
	@rm -f arm7_*.elf

minimal:
	@+$(MAKE) -f Makefile.generic --no-print-directory \
		  NAME=minimal

dswifi:
	@+$(MAKE) -f Makefile.generic --no-print-directory \
		  NAME=dswifi \
		  USE_DSWIFI=1

maxmod:
	@+$(MAKE) -f Makefile.generic --no-print-directory \
		  NAME=maxmod \
		  USE_MAXMOD=1

libxm7:
	@+$(MAKE) -f Makefile.generic --no-print-directory \
		  NAME=libxm7 \
		  USE_LIBXM7=1

dswifi_maxmod:
	@+$(MAKE) -f Makefile.generic --no-print-directory \
		  NAME=dswifi_maxmod \
		  USE_DSWIFI=1 \
		  USE_MAXMOD=1

dswifi_libxm7:
	@+$(MAKE) -f Makefile.generic --no-print-directory \
		  NAME=dswifi_libxm7 \
		  USE_DSWIFI=1 \
		  USE_LIBXM7=1

INSTALLDIR	?= /opt/blocksds/core/sys/arm7/main_core
INSTALLDIR_ABS	:= $(abspath $(INSTALLDIR))

install: all
	@echo "  INSTALL $(INSTALLDIR_ABS)"
	@test $(INSTALLDIR_ABS)
	$(V)$(RM) $(INSTALLDIR_ABS)
	$(V)$(INSTALL) -d $(INSTALLDIR_ABS)
	$(V)$(CP) arm7_*.elf COPYING $(INSTALLDIR_ABS)
