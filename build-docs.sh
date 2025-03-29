#!/bin/bash

# This script must be called from the root of the repository.
#
# This script depends on having hugo in your system.

set -e
set -x

# Download theme

if [ ! -d themes/hugo-geekdoc ]; then
    mkdir -p themes/hugo-geekdoc
    wget https://github.com/thegeeklab/hugo-geekdoc/releases/latest/download/hugo-geekdoc.tar.gz
    tar -xzvf hugo-geekdoc.tar.gz -C themes/hugo-geekdoc --strip-components=1
fi

hugo --cleanDestinationDir --baseURL https://blocksds.skylyrac.net/tutorial/
