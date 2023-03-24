// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

int arm9_pow(int base, unsigned int exponent)
{
    int result = 1;

    for (unsigned int i = 0; i < exponent; i++)
        result *= base;

    return result;
}
