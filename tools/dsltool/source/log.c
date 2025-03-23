// SPDX-License-Identifier: Zlib
//
// Copyright (C) 2025 Antonio Niño Díaz

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#include "log.h"

static log_level curr_log_level = LOG_INFO;

void set_log_level(log_level level)
{
    curr_log_level = level;
}

void print_log(log_level level, const char *msg, ...)
{
    if (curr_log_level < level)
        return;

    va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);
}
