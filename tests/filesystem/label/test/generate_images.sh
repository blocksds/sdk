#!/bin/bash

# Run this script from the test/ folder

BLOCKSDS=/opt/blocksds/core
MKFATIMG=${BLOCKSDS}/tools/mkfatimg/mkfatimg

${MKFATIMG} -t dsi_sd_root dsi_sd_root.img
