// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <stdio.h>
#include <nds.h>

void function(void)
{
    char buffer[5];
    strcpy(buffer, "ABCDEFGHIJK");
    printf("%s\n",buffer);
}

int main(int argc, char *argv[])
{
    defaultExceptionHandler();

    consoleDemoInit();
    function();

    printf("You shouldn't see this message.\n");

    while (1)
       swiWaitForVBlank();
}
