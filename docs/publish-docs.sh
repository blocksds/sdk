#!/bin/bash

# This script must be called from the docs/ folder of the sdk repository.
#
# Make sure that you have a folder called "blocksds-docs" at the same level as
# your sdk repository folder.

rm -rf ../../blocksds-docs/*
mv -v public/* ../../blocksds-docs/

cd ../../blocksds-docs/

touch .nojekyll
git add .
git commit -m"`date`"
git push origin master
