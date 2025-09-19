# BlocksDS library

## 1. Introduction

This library contains information of the version of BlocksDS and macros to
verify that your code is being built in a valid version of BlocksDS. The macros
are available in a C header and in a Makefile that you can include in your own.

## 2. Usage in C

Add the library path to `LIBDIRS` in your Makefile. For example:

```
LIBDIRS		:= $(BLOCKSDS)/libs/libnds \
			   $(BLOCKSDS)/libs/libblocksds
```

Then, you can use it in your code like this, for example:

```c
#include <blocksds_version.h>

#if !BLOCKSDS_VERSION_EQUALS(1, 14, 2)
#error "The version of BlocksDS must be v1.14.2"
#endif

#if !BLOCKSDS_VERSION_AT_LEAST(1, 14, 2)
#error "The installed version of BlocksDS isn't at least v1.14.2"
#endif

int main(int argc, char *argv[])
{
    // ...
    printf("%s\n", BLOCKSDS_VERSION_STRING);
    // ...
}
```

## 2. Usage in Makefile

At the end of your makefile add the following line:

```make
BLOCKSDS_VERSION_FILE = $(BLOCKSDS)/libs/libblocksds/make/blocksds_version.make

ifeq ("$(wildcard $(BLOCKSDS_VERSION_FILE))","")
$(error BlocksDS version too old. Please update.)
endif

include $(BLOCKSDS_VERSION_FILE)
```

After that line you can use it like this, for example:

```make
# This will fail if the installed version of BlocksDS isn't exactly v1.14.2
$(eval $(call blocksds_version_equals, 1, 14, 2))

# This will fail if the installed version of BlocksDS isn't at least v1.14.2
$(eval $(call blocksds_version_at_least, 1, 14, 2))

# This will fail if the installed version of BlocksDS isn't at most v1.14.2
$(eval $(call blocksds_version_at_most, 1, 14, 2))
```
