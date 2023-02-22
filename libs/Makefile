# SPDX-License-Identifier: Zlib
#
# Copyright (c) 2023 Antonio Niño Díaz

CP		:= cp
INSTALL		:= install
MAKE		:= make
RM		:= rm -rf
MKDIR		:= mkdir

.PHONY: all clean dswifi install libnds libsysnds maxmod picolibc

all: dswifi libnds libsysnds maxmod picolibc

dswifi: libnds picolibc
	+$(MAKE) -C dswifi -f Makefile.blocks -j`nproc`

libnds: picolibc
	+$(MAKE) -C libnds -f Makefile.blocks -j`nproc`

libsysnds: picolibc
	+$(MAKE) -C libsysnds -j`nproc`

maxmod: picolibc
	+$(MAKE) -C maxmod -f Makefile.blocks ds -j`nproc`

picolibc: libc

PICOLIBC_INSTALL_PATH	:= $(CURDIR)/libc

# Get the available list of targets with: `arm-none-eabi-gcc --print-multi-lib`
# For information about the build options, picolibc/doc/build.md
libc:
	@cd picolibc && \
	$(RM) build-arm-none-eabi && \
	$(MKDIR) build-arm-none-eabi && \
	cd build-arm-none-eabi && \
	../scripts/do-arm-configure \
		-Dmultilib=false \
		-Dmultilib-list="arm/v5te/softfp;@marm@march=armv5te+fp@mfloat-abi=softfp,thumb/nofp;@mthumb@mfloat-abi=soft" \
		-Dpicocrt=false \
		-Dsemihost=false \
		-Dspecsdir=none \
		-Dtests=false \
		\
		-Dio-long-long=true \
		-Dio-pos-args=true \
		\
		-Dposix-console=true \
		-Dformat-default=double \
		\
		-Dnewlib-nano-malloc=false \
		\
		-Dprefix=$(PICOLIBC_INSTALL_PATH) \
		-Dlibdir=lib \
		-Dincludedir=include && \
	ninja && ninja install

INSTALLDIR_ABS	:= $(abspath $(INSTALLDIR))

install: all
	@echo "INSTALLDIR = '$(INSTALLDIR)' (must not be empty)"
	@test $(INSTALLDIR)
	$(INSTALL) -d $(INSTALLDIR_ABS)/libs \
		      $(INSTALLDIR_ABS)/libs/dswifi \
		      $(INSTALLDIR_ABS)/libs/libnds \
		      $(INSTALLDIR_ABS)/libs/libsysnds \
		      $(INSTALLDIR_ABS)/libs/maxmod
	$(CP) -r dswifi/include dswifi/lib $(INSTALLDIR_ABS)/libs/dswifi
	$(CP) -r libnds/include libnds/lib $(INSTALLDIR_ABS)/libs/libnds
	$(CP) -r libsysnds/lib $(INSTALLDIR_ABS)/libs/libsysnds
	$(CP) -r maxmod/include maxmod/lib $(INSTALLDIR_ABS)/libs/maxmod

clean:
	+$(MAKE) -C dswifi -f Makefile.blocks clean
	+$(MAKE) -C libnds -f Makefile.blocks clean
	+$(MAKE) -C libsysnds clean
	+$(MAKE) -C maxmod -f Makefile.blocks clean
	+$(RM) libc
