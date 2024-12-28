# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023

CP		:= cp
INSTALL		:= install
RM		:= rm -rf
MAKE		:= make
MKDIR		:= mkdir

.PHONY: all clean crts arm7 dldi_r4 install

all: crts arm7 dldi_r4

crts:
	$(MAKE) -C crts

arm7: crts
	$(MAKE) -C arm7
	$(MKDIR) -p default_arm7
	$(CP) arm7/main_core/arm7_dswifi_maxmod.elf default_arm7/arm7.elf
	$(CP) arm7/main_core/COPYING default_arm7/COPYING

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
	$(CP) -r default_arm7 $(INSTALLDIR_ABS)/default_arm7
	$(MAKE) -C crts install INSTALLDIR=$(INSTALLDIR_ABS)/crts
	$(MAKE) -C arm7 install INSTALLDIR=$(INSTALLDIR_ABS)/arm7
	$(MAKE) -C default_makefiles install INSTALLDIR=$(INSTALLDIR_ABS)/default_makefiles

clean:
	$(MAKE) -C crts clean
	$(MAKE) -C arm7 clean
	$(MAKE) -C dldi_r4 clean
	$(RM) default_arm7
