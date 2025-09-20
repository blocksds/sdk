# Returns the number corresponding to the specified blocksds version.
# Use it like this: $(call blocksds_version_to_full, 1, 12, 2)
blocksds_version_to_full = $(shell expr \( $(1) \* 65536 + $(2) \* 256 + $(3) \) )

# Integer that represents the version number of this build
BLOCKSDS_VERSION_FULL = $(call blocksds_version_to_full, ${BLOCKSDS_VERSION_MAJOR}, ${BLOCKSDS_VERSION_MINOR}, ${BLOCKSDS_VERSION_PATCH})

# Returns 1 if the version of BlocksDS matches. Returns 0 otherwise.
# Use it like this: $(call blocksds_version_equals, 1, 12, 2)
blocksds_version_equals = $(shell expr $(BLOCKSDS_VERSION_FULL) == $(call blocksds_version_to_full, $1, $2, $3))

# Returns 1 if the version of BlocksDS is high enough. Returns 0 otherwise.
# Use it like this: $(call blocksds_version_at_least, 1, 12, 2)
blocksds_version_at_least = $(shell expr $(BLOCKSDS_VERSION_FULL) \>= $(call blocksds_version_to_full, $1, $2, $3))

# Returns 1 if the version of BlocksDS is low enough. Returns 0 otherwise.
# Use it like this: $(call blocksds_version_at_most, 1, 12, 2)
blocksds_version_at_most = $(shell expr $(BLOCKSDS_VERSION_FULL) \<= $(call blocksds_version_to_full, $1, $2, $3))

# Causes an error if the version of BlocksDS isn't high enough
# Use it like this: $(eval $(call error_if_blocksds_version_different, 1, 12, 2))
define error_if_blocksds_version_different
    ifneq ($(call blocksds_version_equals, $1, $2, $3), 1)
        $$(error Version of BlocksDS doesn't match)
    endif
endef

# Causes an error if the version of BlocksDS isn't high enough
# Use it like this: $(eval $(call error_if_blocksds_version_less_than, 1, 12, 2))
define error_if_blocksds_version_less_than
    ifneq ($(call blocksds_version_at_least, $1, $2, $3), 1)
        $$(error Version of BlocksDS is too small)
    endif
endef

# Causes an error if the version of BlocksDS isn't low enough
# Use it like this: $(eval $(call error_if_blocksds_version_greater_than, 1, 12, 2))
define error_if_blocksds_version_greater_than
    ifneq ($(call blocksds_version_at_most, $1, $2, $3), 1)
        $$(error Version of BlocksDS is too big)
    endif
endef
