/*
===============================================================================

 Context switching definitions

 Copyright (C) 2021-2023 agbabi contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef _UCONTEXT_H
#define _UCONTEXT_H
#if defined( __cplusplus )
extern "C" {
#endif

#include <sys/ucontext.h>

/**
 * Copies the current machine context into ucp
 * @param ucp Pointer to context structure
 * @return 0
 */
int getcontext(ucontext_t* ucp) __attribute__((nonnull(1)));

/**
 * Sets the current context to ucp
 * @param ucp Pointer to context structure
 * @return Does not return
 */
int setcontext(const ucontext_t* ucp) __attribute__((noreturn, nonnull(1)));

/**
 * Writes current context into oucp, and switches to ucp
 * @param oucp Output address for current context
 * @param ucp Context to swap to
 * @return Although technically this does not return, it will appear to return 0 when switching to oucp
 */
int swapcontext(ucontext_t* __restrict__ oucp, const ucontext_t* __restrict__ ucp) __attribute__((nonnull(1, 2)));

/**
 * Modifies context ucp to invoke func with setcontext. Before invoking, the caller must allocate a new stack for this
 * context and assign its address to ucp->uc_stack, and set a successor context to ucp->uc_link.
 * @param ucp Pointer to context structure
 * @param func Function to invoke with __agbabi_setcontext
 * @param argc Number of arguments passed to func
 * @param ... List of arguments to be passed to func
 */
void makecontext(ucontext_t* ucp, void(*func)(void), int argc, ...) __attribute__((nonnull(1, 2)));

#if defined( __cplusplus )
} // extern "C"
#endif
#endif // define _SYS_UCONTEXT_H
