// SPDX-License-Identifier: Zlib
//
// Copyright (c) 2023 Antonio Niño Díaz

#ifndef FILESYSTEM_H__
#define FILESYSTEM_H__

#include <stdbool.h>

// This initializes NitroFAT. If argv[0] has been set to a non-NULL string, it
// will call fatInitDefault() internally.
//
// The argument 'basepath' is ignored. Please, always pass NULL to it.
//
// This function can be called multiple times, only the first one has any
// effect. Any call after the first one returns the value returned the first
// time.
bool nitroFSInit(char **basepath);

#endif // FILESYSTEM_H__
