#!/bin/bash

pushd plugin_allocator
python3 build.py --clean
popd

pushd plugin_time
python3 build.py --clean
popd

rm -rf application/nitrofs/dsl

pushd application
python3 build.py --clean
popd
