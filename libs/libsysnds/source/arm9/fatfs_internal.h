// SPDX-License-Identifier: Zlib
//
// Copyright (c) 2023 Antonio Niño Díaz

#ifndef FATFS_INTERNAL_H__
#define FATFS_INTERNAL_H__

#include "fatfs/ff.h"

int fatfs_error_to_posix(FRESULT error);

#endif // FATFS_INTERNAL_H__
