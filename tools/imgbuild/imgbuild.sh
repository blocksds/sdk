#!/bin/sh

# Enable error checking
set -e
set -u

IMAGE_PATH=$1
ROOT_DIR_PATH=$2

# If the user provides the size, use it. If not, calculate it.
if [ "$#" -lt 4 ]; then
    SIZE=$(du --bytes --summarize "$ROOT_DIR_PATH" | awk '{print $1}')
    # Add a 5% to the calculated size of all files to make up for blocks that
    # aren't completely full.
    IMAGE_SIZE=$(expr $SIZE \* 105 / 100)
else
    IMAGE_SIZE=$3
fi

# Anything over this size will be formatted as FAT16. Anything below it, as
# FAT12. FAT12 uses space more efficiently, but it has a very low max disk size
# limit.
FAT12_MAX_SIZE=$(expr 32 \* 1024 \* 1024) # 32 MiB

if [ $IMAGE_SIZE -lt $FAT12_MAX_SIZE ]; then
    FAT_VERSION=12
    FAT12_MIN_SIZE=$(expr 64 \* 1024) # 64 KiB is the minimum for FAT12
    IMAGE_SIZE=$(( $IMAGE_SIZE > $FAT12_MIN_SIZE ? $IMAGE_SIZE : $FAT12_MIN_SIZE ))
else
    # The minimum size for FAT16 is 16 MiB
    FAT_VERSION=16
fi

SECTOR_SIZE=512
IMAGE_SIZE_KIB=$(expr $IMAGE_SIZE / 1024)

rm -rf "$IMAGE_PATH"

mkdosfs -F $FAT_VERSION -S $SECTOR_SIZE -C "$IMAGE_PATH" "$IMAGE_SIZE_KIB"
mcopy -i "$IMAGE_PATH" -s "$ROOT_DIR_PATH"/* ::/
