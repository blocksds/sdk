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

MIN_IMAGE_SIZE=$(expr 16 \* 1024 \* 1024) # 16 MiB
IMAGE_SIZE=$(( $IMAGE_SIZE > $MIN_IMAGE_SIZE ? $IMAGE_SIZE : $MIN_IMAGE_SIZE ))
SECTOR_SIZE=512
IMAGE_SIZE_KIB=$(expr $IMAGE_SIZE / 1024)

rm -rf "$IMAGE_PATH"

# Use FAT16 instead of FAT32
mkdosfs -F 16 -S $SECTOR_SIZE -C "$IMAGE_PATH" "$IMAGE_SIZE_KIB"
mcopy -i "$IMAGE_PATH" -s "$ROOT_DIR_PATH"/* ::/
