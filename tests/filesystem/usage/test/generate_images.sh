#!/bin/bash

# Run this script from the test/ folder

BLOCKSDS=/opt/blocksds/core
MKFATIMG=${BLOCKSDS}/tools/mkfatimg/mkfatimg

${MKFATIMG} -t ../sdroot dsi_sd_root.img
