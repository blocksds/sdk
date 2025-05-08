// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

#include <errno.h>
#include <stdio.h>
#include <stddef.h>

#define SYM_PUBLIC  __attribute__((visibility ("default")))
#define SYM_LOCAL   __attribute__((visibility ("hidden")))
#define ARM_CODE    __attribute__((target("arm")))

static int array_1[6] = { 1000, 900, 800, 700, 600, 500 };

static int array_2[6] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };

typedef int (*fpointer)(int, int);

fpointer operation_fn;

// Some things that are present in the main binary
extern __thread int main_binary_tls_int;
int main_binary_arm_function(int a, int b);

ARM_CODE __attribute__((noinline)) int add(int a, int b)
{
    return a + b;
}

int op_add(int a, int b)
{
    // Test that we can call a function from another function within the library
    return add(a, b);
}

int op_sub(int a, int b)
{
    return a - b;
}

int op_mul(int a, int b)
{
    return a * b;
}

int op_div(int a, int b)
{
    // Division requires libgcc
    return a / b;
}

int op_array_1(int a, int b)
{
    if (a < 0 || a >= 6)
        return -1;

    return array_1[a];
}

int op_array_2(int a, int b)
{
    if (a < 0 || a >= 6)
        return -1;

    // Test that we can call printf() from the main binary
    printf("printf from %s(%d, %d)\n", __func__, a, b);

    return array_2[a];
}

SYM_PUBLIC int operation_run(int a, int b)
{
    if (operation_fn == NULL)
        return 0;

    return operation_fn(a, b);
}

SYM_PUBLIC int operation_set(int id)
{
    if ((id < 0) || (id >= 6))
        operation_fn = NULL;

    fpointer ptrs[6] = {
        op_add, op_sub, op_mul, op_div, op_array_1, op_array_2
    };

    operation_fn = ptrs[id];

    return 0;
}

ARM_CODE SYM_PUBLIC int operation_arm(int value)
{
    // Test that we can call ARM and Thumb functions from ARM code. The division
    // will be replaced by a libgcc call by the compiler.
    int res = 333 / value;

    printf("printf from %s(%d)\n"
           "    333 / %d = %d\n",
           __func__, value, value, res);

    return res;
}

SYM_PUBLIC void test_tls_symbols(void)
{
    printf("  main_binary_tls_int = %d\n"
           "  errno = %d\n",
           main_binary_tls_int, errno);
}

ARM_CODE SYM_PUBLIC int arm_tail_call(int a, int b)
{
    __attribute__((musttail)) return main_binary_arm_function(a, b);
}
