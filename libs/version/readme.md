# BlocksDS library

## 1. Introduction

This library contains information of the version of BlocksDS and macros to
verify that your code is being built in a valid version of BlocksDS. The macros
are available in a C header and in a Makefile that you can include in your own.

## 2. Usage in C

Add the library path to `LIBDIRS` in your Makefile. For example:

```
LIBDIRS		:= $(BLOCKSDS)/libs/libnds \
			   $(BLOCKSDS)/libs/version
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

At the end of your makefile, you need to include
`$(BLOCKSDS)/libs/version/blocksds_version.make`.
Below, an example which includes the file and also checks
for older versions of the BlocksDS SDK as well.
Depending on the use case, checking for the existence
of `$(BLOCKSDS)/libs/version/blocksds_version.make` may not be necessary.

```make
BLOCKSDS_VERSION_FILE = $(BLOCKSDS)/libs/version/blocksds_version.make

ifeq ("$(wildcard $(BLOCKSDS_VERSION_FILE))","")
$(error BlocksDS version too old. Please update.)
endif

include $(BLOCKSDS_VERSION_FILE)
```

After those lines you can use it like this, for example:

```make
# This will assign 1 to retval if the installed version of BlocksDS is v1.14.2.
# It will assign 0 otherwise.
retval = $(call blocksds_version_equals, 1, 14, 2)

# This will execute the contents of the "if" when the installed version
# of BlocksDS is at least v1.14.2.
ifeq ($(call blocksds_version_at_least, 1, 14, 2), 1)
...
endif

# This will execute the contents of the "if" when the installed version
# of BlocksDS is greater than v1.14.2.
ifeq ($(call blocksds_version_at_most, 1, 14, 2), 0)
...
endif

# This will fail if the installed version of BlocksDS isn't exactly v1.14.2
$(eval $(call error_if_blocksds_version_different, 1, 14, 2))

# This will fail if the installed version of BlocksDS isn't at least v1.14.2
$(eval $(call error_if_blocksds_version_less_than, 1, 14, 2))

# This will fail if the installed version of BlocksDS isn't at most v1.14.2
$(eval $(call error_if_blocksds_version_greater_than, 1, 14, 2))
```
