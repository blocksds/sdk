/*
===============================================================================

 POSIX:
    makecontext

 Copyright (C) 2021-2023 agbabi contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#include <errno.h>
#include <stdarg.h>
#include <unistd.h>
#include <ucontext.h>

#define REGISTER_ARGS 4

typedef struct {
    const ucontext_t* next[8];
} popcontext_aapcs_stack;

static void popcontext(popcontext_aapcs_stack next) __attribute__((noreturn));

void makecontext(ucontext_t* ucp, void(*func)(void), int argc, ...) {
    if ((unsigned int) argc * sizeof(int) > ucp->uc_stack.ss_size - REGISTER_ARGS * sizeof(int)) {
        errno = ENOMEM;
        return;
    }

    errno = 0;

    unsigned int* funcstack = (unsigned int*) ((uintptr_t) ucp->uc_stack.ss_sp + ucp->uc_stack.ss_size);

    /* Push link context onto stack */
    funcstack -= 1;
    *funcstack = (unsigned int) ucp->uc_link;

    if (argc > REGISTER_ARGS) {
        funcstack -= (argc - REGISTER_ARGS);
    }

    ucp->uc_mcontext.reg_sp = (unsigned int) funcstack;
    ucp->uc_mcontext.reg_lr = (unsigned int) popcontext;
    ucp->uc_mcontext.reg_pc = (unsigned int) func;
    ucp->uc_mcontext.reg_cpsr = ucp->uc_mcontext.reg_pc & 1 ? 0x3f : 0x1f;

    va_list vl;
    va_start(vl, argc);

    int reg;
    unsigned int* regptr = &ucp->uc_mcontext.reg_r0;

    for (reg = 0; reg < argc && reg < REGISTER_ARGS; ++reg) {
        *regptr++ = va_arg(vl, unsigned int);
    }

    for (; reg < argc; ++reg) {
        *funcstack++ = va_arg(vl, unsigned int);
    }

    va_end(vl);
}

void popcontext(const popcontext_aapcs_stack next) {
    if (!next.next[4]) {
        _exit(0);
    } else {
        setcontext(next.next[4]);
    }
}
