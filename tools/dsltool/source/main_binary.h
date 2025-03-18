// SPDX-License-Identifier: Zlib
//
// Copyright (C) 2025 Antonio Niño Díaz

#ifndef MAIN_BINARY_H__
#define MAIN_BINARY_H__

#include <stdbool.h>
#include <stdint.h>

int main_binary_load(const char *path);
bool main_binary_is_loaded(void);
uint32_t main_binary_get_symbol_value(const char *name);
void main_binary_free(void);

#endif // MAIN_BINARY_H__
