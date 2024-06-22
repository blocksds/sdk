#!/bin/bash

# Run this from the root of the repository.
#
# bash scripts/update-version-makefiles.sh 10300

sed -i -E -e "s/(BLOCKSDS_VERSION[[:space:]]+:=[[:space:]]+)[0-9]+/\\1"$1"/g" sys/default_makefiles/*/Makefile* templates/*/Makefile*
