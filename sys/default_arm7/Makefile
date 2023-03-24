# SPDX-License-Identifier: Zlib
#
# Copyright (c) 2023 Antonio Niño Díaz

ifeq ($(strip $(BLOCKSDS)),)
    $(error "Environment variable BLOCKSDS not found")
endif

ARM7_MAJOR	:= 0
ARM7_MINOR	:= 7
ARM7_PATCH	:= 4

VERSTRING	:= $(ARM7_MAJOR).$(ARM7_MINOR).$(ARM7_PATCH)

# Source code paths
# -----------------

SOURCEDIRS	:= source
INCLUDEDIRS	:=

# Defines passed to all files
# ---------------------------

DEFINES		:=

# Libraries
# ---------

LIBS		:= -ldswifi7 -lmm7 -lnds7 -lc
LIBDIRS		:= $(BLOCKSDS)/libs/dswifi \
		   $(BLOCKSDS)/libs/maxmod \
		   $(BLOCKSDS)/libs/libnds

# Build artifacts
# -----------------

BUILDDIR	:= build
NAME		:= arm7
ELF		:= $(NAME).elf
DUMP		:= $(NAME).dump
MAP		:= build/$(NAME).map

# Tools
# -----

PREFIX		:= arm-none-eabi-
CC		:= $(PREFIX)gcc
CXX		:= $(PREFIX)g++
OBJDUMP		:= $(PREFIX)objdump
MKDIR		:= mkdir
RM		:= rm -rf

# Verbose flag
# ------------

ifeq ($(VERBOSE),1)
V		:=
else
V		:= @
endif

# Source files
# ------------

SOURCES_S	:= $(shell find -L $(SOURCEDIRS) -name "*.s")
SOURCES_C	:= $(shell find -L $(SOURCEDIRS) -name "*.c")
SOURCES_CPP	:= $(shell find -L $(SOURCEDIRS) -name "*.cpp")

# Compiler and linker flags
# -------------------------

DEFINES		+= -D__NDS__ -DARM7

ARCH		:= -mcpu=arm7tdmi -mtune=arm7tdmi

WARNFLAGS	:= -Wall

ifeq ($(SOURCES_CPP),)
    LD	:= $(CC)
else
    LD	:= $(CXX)
endif

INCLUDEFLAGS	:= $(foreach path,$(INCLUDEDIRS),-I$(path)) \
		   $(foreach path,$(LIBDIRS),-I$(path)/include)

LIBDIRSFLAGS	:= $(foreach path,$(LIBDIRS),-L$(path)/lib)

ASFLAGS		+= -x assembler-with-cpp $(DEFINES) $(ARCH) \
		   -mthumb -mthumb-interwork $(INCLUDEFLAGS) \
		   -ffunction-sections -fdata-sections

CFLAGS		+= -std=gnu11 $(WARNFLAGS) $(DEFINES) $(ARCH) \
		   -mthumb -mthumb-interwork $(INCLUDEFLAGS) -O2 \
		   -ffunction-sections -fdata-sections \
		   -fomit-frame-pointer

CXXFLAGS	+= -std=gnu++14 $(WARNFLAGS) $(DEFINES) $(ARCH) \
		   -mthumb -mthumb-interwork $(INCLUDEFLAGS) -O2 \
		   -ffunction-sections -fdata-sections \
		   -fno-exceptions -fno-rtti \
		   -fomit-frame-pointer

LDFLAGS		:= -mthumb -mthumb-interwork $(LIBDIRSFLAGS) \
		   -Wl,-Map,$(MAP) -Wl,--gc-sections -nolibc -nostartfiles \
		   -T$(BLOCKSDS)/sys/crts/ds_arm7.ld \
		   -Wl,--no-warn-rwx-segments \
		   -Wl,--start-group $(LIBS) -Wl,--end-group

# Intermediate build files
# ------------------------

OBJS		:= $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_S))) \
		   $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_C))) \
		   $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_CPP)))

DEPS		:= $(OBJS:.o=.d)

# Targets
# -------

.PHONY: all clean dump

all: $(ELF)

$(ELF): $(OBJS)
	@echo "  LD      $@"
	$(V)$(LD) -o $@ $(OBJS) $(BLOCKSDS)/sys/crts/ds_arm7_crt0.o $(LDFLAGS)

$(DUMP): $(ELF)
	@echo "  OBJDUMP $@"
	$(V)$(OBJDUMP) -h -C -S $< > $@

dump: $(DUMP)

clean:
	@echo "  CLEAN"
	$(V)$(RM) $(ELF) $(DUMP) $(MAP) $(BUILDDIR)

# Rules
# -----

$(BUILDDIR)/%.s.o : %.s
	@echo "  AS      $<"
	@$(MKDIR) -p $(@D)
	$(V)$(CC) $(ASFLAGS) -MMD -MP -c -o $@ $<

$(BUILDDIR)/%.c.o : %.c
	@echo "  CC      $<"
	@$(MKDIR) -p $(@D)
	$(V)$(CC) $(CFLAGS) -MMD -MP -c -o $@ $<

$(BUILDDIR)/%.cpp.o : %.cpp
	@echo "  CXX     $<"
	@$(MKDIR) -p $(@D)
	$(V)$(CXX) $(CXXFLAGS) -MMD -MP -c -o $@ $<

# Include dependency files if they exist
# --------------------------------------

-include $(DEPS)
