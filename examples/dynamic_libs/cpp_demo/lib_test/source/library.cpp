// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

#include <stdio.h>

#define SYM_PUBLIC  __attribute__((visibility ("default")))
#define SYM_LOCAL   __attribute__((visibility ("hidden")))

class global_initializer
{
public:
    global_initializer(void)
    {
        printf("Constructor called\n");
    }

    ~global_initializer(void)
    {
        printf("Destructor called\n");
    }

    void print_text(void)
    {
        printf("Class method called\n");
    }
};

global_initializer global_initializer_instance;

SYM_PUBLIC void print_text(void)
{
    global_initializer_instance.print_text();
}
