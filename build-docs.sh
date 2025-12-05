#!/bin/bash

# This script must be called from the root of the repository.
#
# This script depends on having hugo in your system.

set -e
set -x

# Download theme

if [ ! -d themes/hextra ]; then
    cd themes
    git clone https://github.com/imfing/hextra.git --depth=1
    cd hextra
    git checkout 9466af337f3a54e77b68c9852a568b79038ff0ef
    cd ../..
fi

hugo --cleanDestinationDir --baseURL https://blocksds.skylyrac.net/tutorial/
