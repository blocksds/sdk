#!/bin/bash

# This script must be called from the docs/ folder of the sdk repository.
#
# This script depends on having hugo in your system. It has been tested with
# v0.152.2 of hugo-extended.

set -e
set -x

# Download theme

if [ ! -d themes/hextra ]; then
    cd themes
    git clone https://codeberg.org/blocksds/hextra --depth=1
    cd hextra
    git checkout 9466af337f3a54e77b68c9852a568b79038ff0ef
    cd ../..
fi

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

pushd ../tools/grit
make docs
popd
mv ../tools/grit/docs/html public/grit
