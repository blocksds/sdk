// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <stdio.h>
#include <nds.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
#pragma GCC diagnostic ignored "-Wstringop-overflow"

void buggy_function(void)
{
    char buffer[5];
    strcpy(buffer, "ABCDEFGHIJK");
    printf("%s\n",buffer);
}

#pragma GCC diagnostic pop

int main(int argc, char *argv[])
{
    defaultExceptionHandler();

    consoleDemoInit();
    buggy_function();

    printf("You shouldn't see this message.\n");

    while (1)
       swiWaitForVBlank();
}
