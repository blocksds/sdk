// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

#include <stdio.h>

#include <blocksds_version.h>
#include <nds.h>

// You can experiment by changing this version number. If the number is too high
// the project won't be able to be built!
#if !BLOCKSDS_VERSION_AT_LEAST(1, 14, 2)
#error "The version of BlocksDS used to build the project is too old."
#endif

int main(int argc, char **argv)
{
    consoleDemoInit();

    printf("The project has been built!");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        if (keysDown() & KEY_START)
            break;
    }

    return 0;
}
