# SPDX-License-Identifier: Zlib
#
# Copyright (c) 2023 Antonio Niño Díaz

CP		:= cp
INSTALL		:= install
MAKE		:= make
RM		:= rm -rf
MKDIR		:= mkdir

.PHONY: all clean dswifi install libnds maxmod picolibc stdcpp

all: dswifi libnds maxmod picolibc stdcpp

# Build targets
# =============

dswifi: libnds picolibc
	+$(MAKE) -C dswifi -j`nproc`

libnds: picolibc
	+$(MAKE) -C libnds -j`nproc`

maxmod: picolibc
	+$(MAKE) -C maxmod ds -j`nproc`

picolibc: libc9 libc7

LIBC9_INSTALL_PATH	:= $(CURDIR)/libc9
LIBC7_INSTALL_PATH	:= $(CURDIR)/libc7

# Get the available list of targets with: `arm-none-eabi-gcc --print-multi-lib`
# For information about the build options, picolibc/doc/build.md
libc9:
	@cd picolibc && \
	$(RM) build-libc9 && \
	$(MKDIR) build-libc9 && \
	cd build-libc9 && \
	meson setup \
		--cross-file=../../cross-libc9.txt \
		-Dmultilib=false \
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
		-Dprefix=$(LIBC9_INSTALL_PATH) \
		-Dlibdir=lib \
		-Dincludedir=include \
		.. && \
	ninja && ninja install

libc7:
	@cd picolibc && \
	$(RM) build-libc7 && \
	$(MKDIR) build-libc7 && \
	cd build-libc7 && \
	meson setup \
		--cross-file=../../cross-libc7.txt \
		-Dmultilib=false \
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
		-Dprefix=$(LIBC7_INSTALL_PATH) \
		-Dlibdir=lib \
		-Dincludedir=include \
		.. && \
	ninja && ninja install

stdcpp: picolibc
	+$(MAKE) -C stdcpp -j`nproc` install

# Install target
# ==============

INSTALLDIR_ABS	:= $(abspath $(INSTALLDIR))

install: all
	@echo "INSTALLDIR = '$(INSTALLDIR)' (must not be empty)"
	@test $(INSTALLDIR)
	$(INSTALL) -d $(INSTALLDIR_ABS)/libs \
		      $(INSTALLDIR_ABS)/libs/dswifi \
		      $(INSTALLDIR_ABS)/libs/libc7 \
		      $(INSTALLDIR_ABS)/libs/libc9 \
		      $(INSTALLDIR_ABS)/libs/libstdc++7 \
		      $(INSTALLDIR_ABS)/libs/libstdc++9 \
		      $(INSTALLDIR_ABS)/libs/libnds \
		      $(INSTALLDIR_ABS)/libs/maxmod
	$(CP) -r dswifi/include dswifi/lib $(INSTALLDIR_ABS)/libs/dswifi
	$(CP) -r libc7/include libc7/lib $(INSTALLDIR_ABS)/libs/libc7
	$(CP) -r libc9/include libc9/lib $(INSTALLDIR_ABS)/libs/libc9
	$(CP) -r libstdc++7/include libstdc++7/lib $(INSTALLDIR_ABS)/libs/libstdc++7
	$(CP) -r libstdc++9/include libstdc++9/lib $(INSTALLDIR_ABS)/libs/libstdc++9
	$(CP) -r libnds/include libnds/lib $(INSTALLDIR_ABS)/libs/libnds
	$(CP) -r maxmod/include maxmod/lib $(INSTALLDIR_ABS)/libs/maxmod

# Clean target
# ============

clean:
	+$(MAKE) -C dswifi clean
	+$(MAKE) -C libnds clean
	+$(MAKE) -C maxmod clean
	+$(MAKE) -C stdcpp clean
	$(RM) libc7 libc9 libstdc++7 libstdc++9
	$(RM) picolibc/build-libc7 picolibc/build-libc9
