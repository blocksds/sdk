// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

// This example shows how the stack protector of gcc detects that we have
// corrupted the stack after using strcpy() to copy a long string into a buffer
// that can't fit the whole string.
//
// It should show a red crash screen instead of the print statements present in
// the code.

#include <stdio.h>
#include <nds.h>

// gcc really doesn't like it when we try to copy a long string to a small
// buffer with strcpy(). Even if we silence warnings, gcc ignores the strcpy()
// and it calls puts() directly to print the string without any copy.
//
// Using inline assembly helps us get around all the checks.
//
// Also, forcing the function to not be inlined makes it easier to see the stack
// check in the disassembled code (with `make dump`).
__attribute__((noinline)) void buggy_function(void)
{
    char buffer[5];

    register uint32_t r0 asm("r0") = (uintptr_t)&buffer[0];
    register uint32_t r1 asm("r1") = (uintptr_t)(const char *)"ABCDEFGHIJK";

    // The bl is converted to blx by the linker if needed
    asm volatile inline ("bl strcpy" :
                         "+r"(r0), "+r"(r1) ::
                         "r2", "r3", "r12", "memory");

    printf("%s\n",buffer);
}

int main(int argc, char *argv[])
{
    defaultExceptionHandler();

    consoleDemoInit();

    printf("Starting the test...\n");

    buggy_function();

    printf("You shouldn't see this message.\n");

    while (1)
       swiWaitForVBlank();
}
