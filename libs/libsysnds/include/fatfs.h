// SPDX-License-Identifier: Zlib
//
// Copyright (c) 2023 Antonio Niño Díaz

#ifndef FATFS_H__
#define FATFS_H__

#include <stdbool.h>

// This function whether it's running on a regular DS or a DSi.
//
// - DS:  It will try to use DLDI to initialize access to the SD card of the
//        flashcard. If it isn't possible it returns false. If it succeedes, it
//        returns true.
//
// - DSi: It will try to initialize access to the internal SD slot, and the SD
//        of the flashcard. It will only return false if the internal slot of
//        the DSi can't be accessed, and it will return true if it can.
//
// The initial working directory is "fat:/" on the DS (DLDI), and "sd:/" on DSi.
// On the DSi it is possible to switch between both filesystems with `chdir()`.
bool fatInitDefault(void);

#endif // FATFS_H__
