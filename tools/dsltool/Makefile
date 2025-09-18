# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023-2025

# Source code paths
# -----------------

SOURCEDIRS	:= source
INCLUDEDIRS	:= source

# Version string handling
# -----------------------

# Try to generate a version string if it isn't already provided
ifeq ($(VERSION_STRING),)
    # Try an exact match with a tag (e.g. v1.12.1)
    VERSION_STRING	:= $(shell git describe --tags --exact-match --dirty 2>/dev/null)
    ifeq ($(VERSION_STRING),)
        # Try a non-exact match (e.g. v1.12.1-3-g67a811a)
        VERSION_STRING	:= $(shell git describe --tags --dirty 2>/dev/null)
        ifeq ($(VERSION_STRING),)
            # If no version is provided by the user or git, fall back to this
            VERSION_STRING	:= DEV
        endif
    endif
endif

# Defines passed to all files
# ---------------------------

DEFINES		:= -DVERSION_STRING=\"$(VERSION_STRING)\"

# Libraries
# ---------

LIBS		:=
LIBDIRS		:=

# Build artifacts
# ---------------

NAME		:= dsltool
BUILDDIR	:= build
ELF		:= $(NAME)

# Tools
# -----

STRIP		:= -s
BINMODE		:= 755

HOSTCC		?= gcc
HOSTCXX		?= g++
CP		:= cp
MKDIR		:= mkdir
RM		:= rm -rf
MAKE		:= make
INSTALL		:= install

# Verbose flag
# ------------

ifeq ($(VERBOSE),1)
V		:=
else
V		:= @
endif

# Source files
# ------------

SOURCES_C	:= $(shell find -L $(SOURCEDIRS) -name "*.c")
SOURCES_CPP	:= $(shell find -L $(SOURCEDIRS) -name "*.cpp")

# Compiler and linker flags
# -------------------------

WARNFLAGS_C	:= -Wall -Wextra -Wstrict-prototypes -Wshadow

WARNFLAGS_CXX	:= -Wall -Wextra -Wstrict-prototypes -Wshadow

ifeq ($(SOURCES_CPP),)
    HOSTLD	:= $(HOSTCC)
else
    HOSTLD	:= $(HOSTCXX)
endif

INCLUDEFLAGS	:= $(foreach path,$(INCLUDEDIRS),-I$(path)) \
		   $(foreach path,$(LIBDIRS),-I$(path)/include)

LIBDIRSFLAGS	:= $(foreach path,$(LIBDIRS),-L$(path)/lib)

CFLAGS		+= -std=gnu17 $(WARNFLAGS_C) $(DEFINES) $(INCLUDEFLAGS) -O3

CXXFLAGS	+= -std=gnu++14 $(WARNFLAGS_CXX) $(DEFINES) $(INCLUDEFLAGS) -O3

LDFLAGS		+= $(LIBDIRSFLAGS) $(LIBS)

# Intermediate build files
# ------------------------

OBJS		:= $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_C))) \
		   $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_CPP)))

DEPS		:= $(OBJS:.o=.d)

# Targets
# -------

.PHONY: all clean install

all: $(ELF)

$(ELF): $(OBJS)
	@echo "  HOSTLD  $@"
	$(V)$(HOSTLD) -o $@ $(OBJS) $(LDFLAGS)

clean:
	@echo "  CLEAN  "
	$(V)$(RM) $(ELF) $(BUILDDIR)

INSTALLDIR	?= /opt/blocksds/core/tools/dsltool
INSTALLDIR_ABS	:= $(abspath $(INSTALLDIR))

install: all
	@echo "  INSTALL $(INSTALLDIR_ABS)"
	@test $(INSTALLDIR_ABS)
	$(V)$(RM) $(INSTALLDIR_ABS)
	$(V)$(INSTALL) -d $(INSTALLDIR_ABS)
	$(V)$(INSTALL) $(STRIP) -m $(BINMODE) $(NAME) $(INSTALLDIR_ABS)
	$(V)$(CP) ./COPYING $(INSTALLDIR_ABS)

# Rules
# -----

$(BUILDDIR)/%.c.o : %.c
	@echo "  HOSTCC  $<"
	@$(MKDIR) -p $(@D)
	$(V)$(HOSTCC) $(CFLAGS) -MMD -MP -c -o $@ $<

$(BUILDDIR)/%.cpp.o : %.cpp
	@echo "  HOSTCXX $<"
	@$(MKDIR) -p $(@D)
	$(V)$(HOSTCXX) $(CXXFLAGS) -MMD -MP -c -o $@ $<

# Include dependency files if they exist
# --------------------------------------

-include $(DEPS)
