#!/bin/bash

# This script must be called from the docs/ folder of the sdk repository.
#
# Make sure that you have a folder called "blocksds-docs" at the same level as
# your sdk repository folder.

set -e
set -x

# Download theme

if [ ! -d themes/hugo-geekdoc ]; then
    mkdir -p themes/hugo-geekdoc
    curl -L https://github.com/thegeeklab/hugo-geekdoc/releases/latest/download/hugo-geekdoc.tar.gz | tar -xz -C themes/hugo-geekdoc/ --strip-components=1
fi

hugo --cleanDestinationDir --baseURL https://blocksds.github.io/docs/

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

rm -rf ../../blocksds-docs/*
mv -v public/* ../../blocksds-docs/

cd ../../blocksds-docs/

touch .nojekyll
git add .
git commit -m"`date`"
git push origin master
