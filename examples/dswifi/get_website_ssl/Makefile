# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2025

BLOCKSDS	?= /opt/blocksds/core
BLOCKSDSEXT	?= /opt/blocksds/external

# Only build this example if Mbed TLS is found
MBEDTLS_FOUND	:= $(shell find $(BLOCKSDSEXT)/mbedtls -name libmbedtls.a 2> /dev/null)
ifeq ($(strip $(MBEDTLS_FOUND)),)

.PHONY: all clean

all:
	@echo "Error: Mbed TLS not found. Run: wf-pacman -Sy blocksds-mbedtls"

clean:
	@echo "  CLEAN"

else

# User config

NAME		:= dswifi_get_website_ssl
GAME_TITLE	:= Get website SSL
GAME_SUBTITLE	:= DSWiFi

# Source code paths

NITROFSDIR	:= nitrofs

# Libraries

ARM7ELF		:= $(BLOCKSDS)/sys/arm7/main_core/arm7_dswifi_maxmod.elf

LIBS		:= -ldswifi9 -lnds9 -lmbedcrypto -lmbedtls -lmbedx509

LIBDIRS		:= $(BLOCKSDS)/libs/dswifi \
			   $(BLOCKSDS)/libs/libnds \
			   $(BLOCKSDSEXT)/mbedtls

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile

endif
