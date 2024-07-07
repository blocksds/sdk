// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024
// SPDX-FileContributor: Adrian "asie" Siekierka, 2024

// Random file generated with:
//
//     dd bs=1024 count=16 < /dev/urandom > random.bin
//
// Then, update the value ``expected``. One can use the following steps to
// calculate it:
//
// 1. Visit http://www.sunshine2k.de/coding/javascript/crc/crc_js.html
// 2. Paste CRC Input Data as Bytes. Use the following command to convert the
//    binary file to a list of hexadecimal values:
//
//    od -An -t x1 random.bin | sed 's/ / 0x/g'
//
// 3. Configure CRC width to CRC-16 and parametrization to CRC16_MODBUS.
// 4. Press "Calculate CRC!".

#include <stdio.h>

#include <nds.h>

#include "random_bin.h"

int main(int argc, char **argv)
{
    consoleDemoInit();

    uint16_t expected = 0x4125;

    printf("Expected CRC16: %04X\n\n", expected);

    // Calculate CRC-16 code.
    // The 0xFFFF used here is the "initial value" passed to the CRC
    // algorithm.
    uint16_t found = swiCRC16(0xFFFF, random_bin, random_bin_size);

    printf("Calculated CRC16: %04X\n\n", found);
    printf("Do they match? %s\n\n", expected == found ? "Yes" : "No");
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
