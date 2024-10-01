// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <inttypes.h>
#include <stdio.h>

#include <nds.h>

int main(int argc, char **argv)
{
    consoleDemoInit();

    // This example shows that you can leave the divider hardware running
    // asynchronously, start a square root, and eventually get the result of the
    // division.
    div32_asynch(7000, 3);
    uint32_t sqrt_out = sqrt64(12345 * 12345);
    uint32_t div_out = div32_result();

    printf("7000 / 3 = %" PRIu32 "\n", div_out);
    printf("sqrt(12345 ^ 2) = %" PRIu32 "\n", sqrt_out);
    printf("\n");
    printf("Press START to exit");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();
        if (keysDown() & KEY_START)
            break;
    }

    return 0;
}
