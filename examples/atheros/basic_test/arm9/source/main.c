// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2026

#include <stdio.h>

#include <nds.h>

volatile int result;
volatile int error_code;

void fifo_handler_value32_response(u32 value, void *userdata)
{
    result = value;
}

void fifo_handler_value32_error_code(u32 value, void *userdata)
{
    error_code = value;
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    if (!isDSiMode())
    {
        printf("Only available on DSi\n\n");
        printf("Press START to exit");

        while (1)
        {
            swiWaitForVBlank();

            scanKeys();
            if (keysHeld() & KEY_START)
                return 0;
        }
    }

    fifoSetValue32Handler(FIFO_USER_01, fifo_handler_value32_response, 0);
    fifoSetValue32Handler(FIFO_USER_02, fifo_handler_value32_error_code, 0);

    int value = 8;

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint16_t keys = keysDown();
        if (keys & KEY_START)
            break;

        if ((keys & KEY_UP) && (value < 15))
        {
            value++;
            fifoSendValue32(FIFO_USER_01, value);
        }
        else if ((keys & KEY_DOWN) && (value > 0))
        {
            value--;
            fifoSendValue32(FIFO_USER_01, value);
        }

        consoleClear();

        printf("UP/DOWN: Change value\n\n");
        printf("Valid values are 0 to 10\n\n");
        printf("Press START to exit\n\n");

        printf("Value:       %d\n", value);
        printf("Result (x3): %d\n", result);
        printf("Error code:  %d\n", error_code);
    }

    return 0;
}
