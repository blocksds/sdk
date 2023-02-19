# SPDX-License-Identifier: Zlib
#
# Copyright (c) 2023 Antonio Niño Díaz

STRIP		:= -s
BINMODE		:= 755

MAKE		:= make
INSTALL		:= install

.PHONY: clean grit install mmutil ndstool

all: grit mmutil ndstool

grit: grit/grit

grit/grit:
	cd grit && ./autogen.sh && ./configure && make -j`nproc`

mmutil: mmutil/mmutil

mmutil/mmutil:
	cd mmutil && ./autogen.sh && ./configure && make -j`nproc`

ndstool: ndstool/ndstool

ndstool/ndstool:
	cd ndstool && ./autogen.sh && ./configure && make -j`nproc`

INSTALLDIR_ABS	:= $(abspath $(INSTALLDIR))

install: all
	@echo "INSTALLDIR = '$(INSTALLDIR)' (must not be empty)"
	@test $(INSTALLDIR)
	$(INSTALL) -d $(INSTALLDIR_ABS)/tools \
		      $(INSTALLDIR_ABS)/tools/grit \
		      $(INSTALLDIR_ABS)/tools/mmutil \
		      $(INSTALLDIR_ABS)/tools/ndstool
	$(INSTALL) $(STRIP) -m $(BINMODE) grit/grit $(INSTALLDIR_ABS)/tools/grit/
	$(INSTALL) $(STRIP) -m $(BINMODE) mmutil/mmutil $(INSTALLDIR_ABS)/tools/mmutil/
	$(INSTALL) $(STRIP) -m $(BINMODE) ndstool/ndstool $(INSTALLDIR_ABS)/tools/ndstool/

clean:
	$(MAKE) -C grit distclean
	$(MAKE) -C mmutil distclean
	$(MAKE) -C ndstool distclean
