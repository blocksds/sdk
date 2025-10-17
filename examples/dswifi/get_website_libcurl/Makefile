# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2025

BLOCKSDS	?= /opt/blocksds/core
BLOCKSDSEXT	?= /opt/blocksds/external
WONDERFUL_TOOLCHAIN	?= /opt/wonderful

# Only build this example if Mbed TLS, libcurl and zlib are found
MBEDTLS_FOUND	:= $(shell find $(BLOCKSDSEXT)/mbedtls -name libmbedtls.a 2> /dev/null)
LIBCURL_FOUND	:= $(shell find $(BLOCKSDSEXT)/libcurl -name libcurl.a 2> /dev/null)
ZLIB_FOUND	:= $(shell find $(WONDERFUL_TOOLCHAIN)/toolchain/gcc-arm-none-eabi/arm-none-eabi/lib/ -name libz.a 2> /dev/null)

ifneq ($(strip $(MBEDTLS_FOUND)),)
    ifneq ($(strip $(LIBCURL_FOUND)),)
        ifneq ($(strip $(ZLIB_FOUND)),)
            PACKAGES_FOUND	:= true
        endif
    endif
endif


ifneq ($(strip $(PACKAGES_FOUND)),true)

all:
	@echo "Error: Packages missing. Run this command:"
	@echo "    wf-pacman -Sy toolchain-gcc-arm-none-eabi-zlib blocksds-mbedtls blocksds-libcurl"

else

# User config

NAME		:= dswifi_libcurl
GAME_TITLE	:= Using libcurl
GAME_SUBTITLE	:= DSWiFi

# Libraries

ARM7ELF		:= $(BLOCKSDS)/sys/arm7/main_core/arm7_dswifi_maxmod.elf

LIBS		:= -ldswifi9 -lnds9 -lmbedcrypto -lmbedtls -lmbedx509 -lcurl -lz

LIBDIRS		:= $(BLOCKSDS)/libs/dswifi \
			   $(BLOCKSDS)/libs/libnds \
			   $(BLOCKSDSEXT)/mbedtls \
			   $(BLOCKSDSEXT)/libcurl

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile

endif
