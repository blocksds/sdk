#!/bin/bash

# This script must be called from the docs/ folder of the sdk repository.
#
# This script depends on having hugo in your system.

set -e
set -x

# Download theme

if [ ! -d themes/hextra ]; then
    mkdir -p themes
    cd themes
    git clone https://github.com/imfing/hextra.git --depth=1
fi
exit 0

hugo --cleanDestinationDir --baseURL https://blocksds.skylyrac.net/

pushd ../libs/libnds
make docs
popd
mv ../libs/libnds/docs/html public/libnds

pushd ../libs/libteak
make docs
popd
mv ../libs/libteak/docs/html public/libteak

pushd ../libs/libxm7
make docs
popd
mv ../libs/libxm7/docs/html public/libxm7

pushd ../libs/dswifi
make docs
popd
mv ../libs/dswifi/docs/html public/dswifi

pushd ../libs/maxmod
make docs
popd
mv ../libs/maxmod/docs/html public/maxmod
