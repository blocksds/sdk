// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <stdio.h>

#include <nds.h>

int main(int argc, char **argv)
{
    consoleDemoInit();

    // Clear console
    printf("\x1b[2J");
    printf("Raw command line\n");
    printf("----------------\n");
    printf("\n");
    printf("Length: %d bytes\n", __system_argv->length);
    printf("\n");

    printf("[");
    for (int i = 0; i < __system_argv->length; i++)
    {
        char c = __system_argv->commandLine[i];
        if (c < 32)
            c = '|';
        printf("%c", c);
    }
    printf("]\n");

    printf("\n");
    printf("\n");
    printf("Parsed command line\n");
    printf("-------------------\n");
    printf("\n");
    printf("argc: %d\n", argc);
    printf("\n");
    for (int i = 0; i < argc; i++)
        printf("[%s]\n", argv[i]);
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("Press START to exit\n");

    fflush(stdout);

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint32_t keys_down = keysDown();
        if (keys_down & KEY_START)
            break;
    }

    return 0;
}
