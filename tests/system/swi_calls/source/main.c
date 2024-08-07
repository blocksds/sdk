// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <stdio.h>

#include <nds.h>

int main(int argc, char **argv)
{
    consoleDemoInit();

    printf("swiDivide(2000, 7): %d\n", swiDivide(2000, 7));
    printf("swiRemainder(2000, 7): %d\n", swiRemainder(2000, 7));
    int result, remainder;
    swiDivMod(2000, 7, &result, &remainder);
    printf("swiDivMod(2000, 7): %d, %d\n", result, remainder);
    printf("swiSqrt(3000): %d\n", swiSqrt(3000));
    printf("\n");

    // CRC-16/ARC: It should return 0x3703
    const char crc16_test[] =
        "This is a test string to test the CRC-16 functionality of the BIOS";
    unsigned int crc = swiCRC16(0, crc16_test, strlen(crc16_test));
    printf("swiCRC16(): 0x%04X\n", crc);

    const char src_buffer[] = "This is a test string";
    char dst_buffer[50] = { 0 };
    size_t len = strlen(src_buffer);
    swiCopy(src_buffer, dst_buffer, len);
    printf("swiCopy(): %s\n",
           memcmp(src_buffer, dst_buffer, len) == 0 ? "OK": "FAIL");

    if (isDSiMode())
        printf("swiIsDebugger(): Not in DSi\n");
    else
        printf("swiIsDebugger(): %d\n", swiIsDebugger());

    printf("\n");
    printf("swiDelay()");
    for (int i = 0; i < 10; i++)
    {
        swiDelay(0xFFFFFF);
        printf(".");
    }
    printf("\n");
    printf("\n");

    printf("SELECT: Soft reset\n");
    printf("START: Exit to loader\n");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint32_t keys_down = keysDown();
        if (keys_down & KEY_START)
            break;

        if (keys_down & KEY_SELECT)
            swiSoftReset();
    }

    return 0;
}
