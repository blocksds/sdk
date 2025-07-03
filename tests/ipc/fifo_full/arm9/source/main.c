// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

// This test lets you send packets of all types from the ARM9 and receive them
// from the ARM7. The test is designed to fill the FIFO queues (both hardware
// and software queues) to check that the system can handle it.

#include <stdio.h>

#include <nds.h>

unsigned int current_value32 = 0;
unsigned int current_address = 0;
unsigned int current_data = 0;

void send_value32_packets(void)
{
    printf("Sending value32 packets\n");

    for (unsigned int i = 0; i < 256; i++)
    {
        printf("%u ", current_value32);

        if (fifoSendValue32(FIFO_USER_01, current_value32) == false)
        {
            printf("\nCan't send %u\n", current_value32);
            break;
        }

        consoleArm7Flush();
        current_value32++;
    }

    consoleArm7Flush();
    printf("\n");
    printf("Done\n");
}

void send_address_packets(void)
{
    printf("Sending address packets\n");

    for (unsigned int i = 0; i < 256; i++)
    {
        printf("%u ", current_address);

        char *p = (char *)(0x2000000 + current_address);
        if (fifoSendAddress(FIFO_USER_01, p) == false)
        {
            printf("\nCan't send %u\n", current_address);
            break;
        }

        consoleArm7Flush();
        current_address++;
    }

    consoleArm7Flush();
    printf("\n");
    printf("Done\n");
}

void send_data_packets(void)
{
    printf("Sending data packets\n");

    for (unsigned int i = 0; i < 128; i++)
    {
        printf("%u ", current_data);

        unsigned int msg[2];
        msg[0] = current_data;
        msg[1] = 0x55AA1188; // Magic number

        if (fifoSendDatamsg(FIFO_USER_01, sizeof(msg), (uint8_t*)&msg) == false)
        {
            printf("\nCan't send %u\n", current_data);
            break;
        }

        consoleArm7Flush();
        current_data++;
    }

    consoleArm7Flush();
    printf("\n");
    printf("Done\n");
}

int main(int argc, char **argv)
{
    PrintConsole topScreen;
    PrintConsole bottomScreen;

    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(&topScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    consoleInit(&bottomScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);

    // Setup ARM7 console
    // ------------------

    consoleSelect(&bottomScreen);

    // Use bright green
    consoleSetColor(&bottomScreen, CONSOLE_LIGHT_GREEN);

    // Print ARM7 controls
    consoleSetWindow(&bottomScreen, 0, 0, 32, 5);

    printf("R: Receive value32 packets\n");
    printf("A: Receive address packets\n");
    printf("B: Receive data packets\n");

    consoleSetCursor(NULL, 5, 4);
    printf("_________ARM7_________");

    // Reduce the part of the screen used for printing logs
    consoleSetWindow(&bottomScreen, 0, 6, 32, 18);

    consoleArm7Setup(&bottomScreen, 16 * 1024);

    // Wait for the ARM7 to handle the console setup command
    swiWaitForVBlank();
    swiWaitForVBlank();

    // Switch back to the ARM9 console
    // -------------------------------

    consoleSelect(&topScreen);

    // Print ARM9 controls
    consoleSetWindow(&topScreen, 0, 0, 32, 5);

    printf("L:     Send value32 packets\n");
    printf("LEFT:  Send address packets\n");
    printf("DOWN:  Send data packets\n");
    printf("START: Exit\n ");
    printf("    _________ARM9_________");

    // Reduce the part of the screen used for printing logs
    consoleSetWindow(&topScreen, 0, 6, 32, 18);

    while (1)
    {
        swiWaitForVBlank();

        // Print messages that may have taken a bit longer to be printed
        consoleArm7Flush();

        scanKeys();

        uint16_t keys = keysDown();
        if (keys & KEY_START)
            break;

        if (keys & KEY_L)
            send_value32_packets();
        if (keys & KEY_LEFT)
            send_address_packets();
        if (keys & KEY_DOWN)
            send_data_packets();
    }

    return 0;
}
