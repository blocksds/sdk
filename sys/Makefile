# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023

CP		:= cp
INSTALL		:= install
RM		:= rm -rf
MAKE		:= make

.PHONY: all clean crts default_arm7 dldi_r4 install

all: crts default_arm7 dldi_r4

crts:
	$(MAKE) -C crts

default_arm7: crts
	$(MAKE) -C default_arm7

dldi_r4:
	$(MAKE) -C dldi_r4

INSTALLDIR	?= /opt/blocksds/core/sys
INSTALLDIR_ABS	:= $(abspath $(INSTALLDIR))

install: all
	@echo "  INSTALL $(INSTALLDIR_ABS)"
	@test $(INSTALLDIR_ABS)
	@$(RM) $(INSTALLDIR_ABS)
	@$(INSTALL) -d $(INSTALLDIR_ABS) \
		      $(INSTALLDIR_ABS)/dldi_r4
	$(INSTALL) dldi_r4/r4tf.dldi dldi_r4/LICENSE $(INSTALLDIR_ABS)/dldi_r4/
	$(CP) icon.bmp $(INSTALLDIR_ABS)
	$(MAKE) -C crts install INSTALLDIR=$(INSTALLDIR_ABS)/crts
	$(MAKE) -C default_arm7 install INSTALLDIR=$(INSTALLDIR_ABS)/default_arm7
	$(MAKE) -C default_makefiles install INSTALLDIR=$(INSTALLDIR_ABS)/default_makefiles

clean:
	$(MAKE) -C crts clean
	$(MAKE) -C default_arm7 clean
	$(MAKE) -C dldi_r4 clean
