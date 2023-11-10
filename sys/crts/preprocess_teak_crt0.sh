#!/bin/bash

# This script preprocesses ds_teak_crt0.original.s to generate ds_teak_crt0.s
# because the LLVM Teak assembler can't evaluate expressions that are used as
# arguments of instructions.

# Expand macros
export BLOCKSDS=/opt/blocksds/core
cpp -I"$BLOCKSDS"/libs/libnds/include ds_teak_crt0.original.s -o ds_teak_crt0.cpp.s

# Evaluate expressions
python3 preprocess_teak_crt0.py --input ds_teak_crt0.cpp.s --output ds_teak_crt0.s
