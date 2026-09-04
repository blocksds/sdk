#!/bin/bash

set -e

bash clean.sh

pushd plugin_allocator
python3 build.py
popd

pushd plugin_time
python3 build.py
popd

mkdir -p application/nitrofs/dsl

cp plugin_allocator/allocator.dsl application/nitrofs/dsl
cp plugin_time/time.dsl application/nitrofs/dsl

pushd application
python3 build.py
popd

cp application/*.nds .
