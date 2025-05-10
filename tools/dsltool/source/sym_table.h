// SPDX-License-Identifier: Zlib
//
// Copyright (C) 2025 Antonio Niño Díaz

#ifndef SYM_TABLE_H__
#define SYM_TABLE_H__

#include <stdint.h>
#include <stdio.h>

void sym_add_to_table(const char *name, uint32_t value, bool is_public, bool is_unknown);
int sym_set_as_used(unsigned int index);
int sym_set_as_public(unsigned int index);

bool sym_is_unknown(unsigned int index);
const char *sym_get_name(unsigned int index);
int sym_get_index_from_name(const char *name);
int sym_get_sym_index_by_old_index(unsigned int index);

void sym_clear_unused(void);
void sym_sort_table(void);
void sym_clear_table(void);
void sym_print_table(void);
int sym_table_save_to_file(FILE *f);

#endif // SYM_TABLE_H__
