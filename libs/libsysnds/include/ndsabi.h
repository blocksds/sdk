/*
===============================================================================

 C header file for ndsabi

 Copyright (C) 2021-2023 agbabi contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef NDSABI_H
#define NDSABI_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/**
 * Copies n bytes from src to dest (forward)
 * Assumes dest and src are 2-byte aligned
 * @param dest Destination address
 * @param src Source address
 * @param n Number of bytes to copy
 */
void __ndsabi_memcpy2(void* __restrict__ dest, const void* __restrict__ src, size_t n) __attribute__((nonnull(1, 2)));

/**
 * Copies n bytes from src to dest (forward)
 * This is a slow, unaligned, byte-by-byte copy: ideal for SRAM
 * @param dest Destination address
 * @param src Source address
 * @param n Number of bytes to copy
 */
void __ndsabi_memcpy1(void* __restrict__ dest, const void* __restrict__ src, size_t n) __attribute__((nonnull(1, 2)));

/**
 * Copies n bytes from src to dest (backwards)
 * This is a slow, unaligned, byte-by-byte copy: ideal for SRAM
 * @param dest Destination address
 * @param src Source address
 * @param n Number of bytes to copy
 */
void __ndsabi_rmemcpy1(void* __restrict__ dest, const void* __restrict__ src, size_t n) __attribute__((nonnull(1, 2)));

/**
 * Copies n bytes from src to dest (backwards)
 * @param dest Destination address
 * @param src Source address
 * @param n Number of bytes to copy
 */
void __ndsabi_rmemcpy(void* __restrict__ dest, const void* __restrict__ src, size_t n) __attribute__((nonnull(1, 2)));

#ifdef __cplusplus
}
#endif
#endif /* define NDSABI_H */
