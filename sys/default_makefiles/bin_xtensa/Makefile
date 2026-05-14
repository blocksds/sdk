# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023-2026

export BLOCKSDS			?= /opt/blocksds/core
export BLOCKSDSEXT		?= /opt/blocksds/external

export WONDERFUL_TOOLCHAIN	?= /opt/wonderful
XTENSA_ELF_PATH	?= $(WONDERFUL_TOOLCHAIN)/toolchain/gcc-xtensa-elf/bin/

# User config
# ===========

NAME		?= template

# A compile_commands.json file is created if this is set to 1
COMPDB		?= 0

# Source code paths
# -----------------

SOURCEDIRS	?= source
INCLUDEDIRS	?=
BINDIRS		?=

# Defines passed to all files
# ---------------------------

DEFINES		?=

# Libraries
# ---------

LIBS		?= -lath6k
LIBDIRS		+= $(BLOCKSDS)/libs/libath6k

# Build artifacts
# ---------------

BUILDDIR	:= build/$(NAME)
ELF		:= build/$(NAME).elf
DUMP		:= build/$(NAME).dump
MAP		:= build/$(NAME).map
BIN		:= build/$(NAME).bin

# Tools
# -----

PREFIX		:= $(XTENSA_ELF_PATH)xtensa-elf-
CC		:= $(PREFIX)gcc
CXX		:= $(PREFIX)g++
LD		:= $(PREFIX)gcc
OBJDUMP		:= $(PREFIX)objdump
OBJCOPY		:= $(PREFIX)objcopy
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

ifneq ($(BINDIRS),)
    SOURCES_BIN	:= $(shell find -L $(BINDIRS) -name "*.bin")
    INCLUDEDIRS	+= $(addprefix $(BUILDDIR)/,$(BINDIRS))
endif

SOURCES_S	+= $(shell find -L $(SOURCEDIRS) -name "*.s")
SOURCES_C	+= $(shell find -L $(SOURCEDIRS) -name "*.c")
SOURCES_CPP	+= $(shell find -L $(SOURCEDIRS) -name "*.cpp")

# Compiler and linker flags
# -------------------------

DEFINES		+= -D__NDS__ -D__BLOCKSDS__ -DXTENSA

ARCH		:=

WARNFLAGS	?= -Wall -Wextra -Wstrict-prototypes -Wshadow

#ifeq ($(SOURCES_CPP),)
#	LIBS	+= -lc
#else
#	LIBS	+= -lstdc++ -lc
#endif

INCLUDEFLAGS	:= $(foreach path,$(INCLUDEDIRS),-I$(path)) \
		   $(foreach path,$(LIBDIRS),-I$(path)/include)

LIBDIRSFLAGS	:= $(foreach path,$(LIBDIRS),-L$(path)/lib)

ASFLAGS		:= -x assembler-with-cpp $(INCLUDEFLAGS) $(DEFINES) \
		   $(ARCH) -ffunction-sections -fdata-sections \
		   --abi-windowed $(ASFLAGS)

CFLAGS		:= $(WARNFLAGS) $(INCLUDEFLAGS) $(DEFINES) \
		   $(ARCH) -O2 -ffunction-sections -fdata-sections \
		   -ffreestanding -mabi=windowed $(CFLAGS)

CXXFLAGS	:= $(WARNFLAGS) $(INCLUDEFLAGS) $(DEFINES) \
		   $(ARCH) -O2 -ffunction-sections -fdata-sections \
		   -fno-exceptions -fno-rtti \
		   -ffreestanding -mabi=windowed $(CXXFLAGS)

LDFLAGS		:= $(ARCH) $(LIBDIRSFLAGS) -Wl,-Map,$(MAP) $(DEFINES) \
		   -Wl,--start-group $(LIBS) -Wl,--end-group \
		   -T $(BLOCKSDS)/libs/libath6k/ath6k.ld \
		   -nostdlib -nostartfiles -Wl,--gc-sections \
		   $(LDFLAGS)

# Intermediate build files
# ------------------------

OBJS_ASSETS	:= $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_BIN)))

HEADERS_ASSETS	:= $(patsubst %.bin,%_bin.h,$(addprefix $(BUILDDIR)/,$(SOURCES_BIN)))

OBJS_SOURCES	:= $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_S))) \
		   $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_C))) \
		   $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_CPP)))

OBJS		:= $(OBJS_ASSETS) $(OBJS_SOURCES)

DEPS		:= $(OBJS:.o=.d)

# Targets
# -------

.PHONY: all clean dump

all: $(BIN)

$(BIN): $(ELF)
	@echo "  OBJCOPY $@"
	$(V)$(OBJCOPY) -O binary $< $@

$(ELF): $(OBJS)
	@echo "  LD      $@"
	$(V)$(LD) -o $@ $(OBJS) $(LDFLAGS)

$(DUMP): $(ELF)
	@echo "  OBJDUMP   $@"
	$(V)$(OBJDUMP) -h -C -S $< > $@

dump: $(DUMP)

clean:
	@echo "  CLEAN"
	$(V)$(RM) $(BIN) $(DUMP) build compile_commands.json

ifeq ($(COMPDB),1)
# Add an additional dependency to the "all" rule
all: compile_commands.json

compile_commands.json: $(OBJS)
	@echo "  MERGE   compile_commands.json"
	$(V)$(WONDERFUL_TOOLCHAIN)/bin/wf-compile-commands-merge $@ $(patsubst %.o,%.cc.json,$^)
endif

# Rules
# -----

ifeq ($(COMPDB),1)

$(BUILDDIR)/%.s.o : %.s
	@echo "  AS      $<"
	@$(MKDIR) -p $(@D)
	$(V)$(CC) $(ASFLAGS) -MMD -MP -c -MJ $(patsubst %.o,%.cc.json,$@) -o $@ $<

$(BUILDDIR)/%.c.o : %.c
	@echo "  CC      $<"
	@$(MKDIR) -p $(@D)
	$(V)$(CC) $(CFLAGS) -MMD -MP -c -MJ $(patsubst %.o,%.cc.json,$@) -o $@ $<

$(BUILDDIR)/%.cpp.o : %.cpp
	@echo "  CXX     $<"
	@$(MKDIR) -p $(@D)
	$(V)$(CXX) $(CXXFLAGS) -MMD -MP -c -MJ $(patsubst %.o,%.cc.json,$@) -o $@ $<

else

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

endif

$(BUILDDIR)/%.bin.o $(BUILDDIR)/%_bin.h : %.bin
	@echo "  BIN2C   $<"
	@$(MKDIR) -p $(@D)
	$(V)$(BLOCKSDS)/tools/bin2c/bin2c $< $(@D)
	$(V)$(CC) $(CFLAGS) -MMD -MP -c -o $(BUILDDIR)/$*.bin.o $(BUILDDIR)/$*_bin.c

# All assets must be built before the source code
# -----------------------------------------------

$(SOURCES_S) $(SOURCES_C) $(SOURCES_CPP): $(HEADERS_ASSETS)

# Include dependency files if they exist
# --------------------------------------

-include $(DEPS)
