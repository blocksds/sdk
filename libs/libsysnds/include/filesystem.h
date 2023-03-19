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

// When NitroFAT detects it's running in an official cartridge, or in an
// emulator, this function lets you define the CPU that is in charge of reading
// from the cart. By default the ARM7 is in charge of reading the cart. This
// function lets you switch to using the ARM9 as well. You may switch between
// CPUs at runtime, but be careful to not switch while the card is being read.
void nitroFATSetReaderCPU(bool use_arm9);

#endif // FILESYSTEM_H__
