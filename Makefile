# SPDX-License-Identifier: Zlib
#
# Copyright (c) 2023 Antonio Niño Díaz

.PHONY: all clean install libs sys tools

all: libs sys tools

MAKE		:= make

# libnds depends on grit and bin2c
libs: tools
	+$(MAKE) -C libs

# The default ARM7 depends on some libraries
sys: libs
	+$(MAKE) -C sys

tools:
	+$(MAKE) -C tools

install: all
	@echo "INSTALLDIR = '$(INSTALLDIR)' (must not be empty)"
	@test $(INSTALLDIR)
	+$(MAKE) -C libs install INSTALLDIR=$(abspath $(INSTALLDIR))
	+$(MAKE) -C sys install INSTALLDIR=$(abspath $(INSTALLDIR))
	+$(MAKE) -C tools install INSTALLDIR=$(abspath $(INSTALLDIR))

clean:
	+$(MAKE) -C libs clean
	+$(MAKE) -C sys clean
	+$(MAKE) -C tools clean
