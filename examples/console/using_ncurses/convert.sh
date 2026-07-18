#!/bin/sh

rm -rf nitrofs/terminfo
mkdir -p nitrofs/terminfo

tic -s -x -v3 -onitrofs/terminfo libnds.ti
