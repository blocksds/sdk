// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: ds-sloth, 2024

#include "cross_platform_logic.h"

#include <stdio.h>

int main(int argc, char **argv)
{
    int result = cross_platform_program();

    printf("The answer is... %d\n", result);

    return 0;
}
