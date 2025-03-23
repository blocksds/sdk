// SPDX-License-Identifier: Zlib
//
// Copyright (C) 2025 Antonio Niño Díaz

#ifndef LOG_H__
#define LOG_H__

typedef enum {
    LOG_ERROR = 0,
    LOG_INFO = 1,
    LOG_VERBOSE = 2,
} log_level;

void set_log_level(log_level level);
void print_log(log_level level, const char *msg, ...);

#define ERROR(...)      print_log(LOG_ERROR, __VA_ARGS__)
#define INFO(...)       print_log(LOG_INFO, __VA_ARGS__)
#define VERBOSE(...)    print_log(LOG_VERBOSE, __VA_ARGS__)

#endif // LOG_H__
