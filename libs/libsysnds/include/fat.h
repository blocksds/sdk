// SPDX-License-Identifier: Zlib
//
// Copyright (c) 2023 Antonio Niño Díaz

#ifndef FAT_H__
#define FAT_H__

#include <stdbool.h>

// This function works whether it's running on a regular DS or a DSi:
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
//
// This function can be called multiple times, only the first one has any
// effect. Any call after the first one returns the value returned the first
// time.
bool fatInitDefault(void);

#endif // FAT_H__
