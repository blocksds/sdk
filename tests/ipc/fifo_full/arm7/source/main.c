// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023-2025

// This file doesn't use consoleFlush() or inputGetAndSend() so that all the
// FIFO packets sent during the test are the test packets.

#include <nds.h>

volatile bool exit_loop = false;

void power_button_callback(void)
{
    exit_loop = true;
}

void vblank_handler(void)
{
    //inputGetAndSend();
}

unsigned int expected_value32 = 0;
unsigned int expected_address = 0x2000000;
unsigned int expected_data = 0;

void check_value32_packets(void)
{
    consolePuts("Checking value32\n");

    if (fifoCheckValue32(FIFO_USER_01) == false)
    {
        consolePuts("Empty\n");
        return;
    }

    while (fifoCheckValue32(FIFO_USER_01))
    {
        unsigned int value = fifoGetValue32(FIFO_USER_01);
        if (value == expected_value32)
        {
            consolePrintf("%u ", value);
            expected_value32++;
        }
        else
        {
            consolePuts("\nValue32 check failed!\n");
            consolePrintf("Got %u, expected %u", value, expected_value32);
            while (1)
                swiWaitForVBlank();
        }
    }

    consolePuts("\nNo more left\n");
    consolePuts("\n");
}

void check_address_packets(void)
{
    consolePuts("Checking address\n");

    if (fifoCheckAddress(FIFO_USER_01) == false)
    {
        consolePuts("Empty\n");
        return;
    }

    while (fifoCheckAddress(FIFO_USER_01))
    {
        unsigned int value = (unsigned int)fifoGetAddress(FIFO_USER_01);
        if (value == expected_address)
        {
            consolePrintf("%u ", value & 0xFFFF);
            expected_address++;
        }
        else
        {
            consolePuts("\nAddress check failed!\n");
            consolePrintf("Got %u, expected %u", value & 0xFFFF,
                          expected_address & 0xFFFF);
            while (1)
                swiWaitForVBlank();
        }
    }

    consolePuts("\nNo more left\n");
    consolePuts("\n");
}

void check_data_packets(void)
{
    consolePuts("Checking data\n");

    if (fifoCheckDatamsg(FIFO_USER_01) == false)
    {
        consolePuts("Empty\n");
        return;
    }

    while (fifoCheckDatamsg(FIFO_USER_01))
    {
        unsigned int msg[2];

        int size = (unsigned int)fifoGetDatamsg(FIFO_USER_01, sizeof(msg), (uint8_t *)msg);
        if (size != sizeof(msg))
        {
            consolePuts("Invalid size!");
            while (1)
                swiWaitForVBlank();
        }

        if (msg[1] != 0x55AA1188)
        {
            consolePuts("Invalid magic!");
            while (1)
                swiWaitForVBlank();
        }

        if (msg[0] == expected_data)
        {
            consolePrintf("%u ", msg[0]);
            expected_data++;
        }
        else
        {
            consolePuts("\nData check failed!\n");
            consolePrintf("Got %u, expected %u", msg[0], expected_data);
            while (1)
                swiWaitForVBlank();
        }
    }

    consolePuts("\nNo more left\n");
    consolePuts("\n");
}

int main(int argc, char *argv[])
{
    // Initialize sound hardware
    enableSound();

    // Read user information from the firmware (name, birthday, etc)
    readUserSettings();

    // Stop LED blinking
    ledBlink(LED_ALWAYS_ON);

    // Using the calibration values read from the firmware with
    // readUserSettings(), calculate some internal values to convert raw
    // coordinates into screen coordinates.
    touchInit();

    irqInit();
    fifoInit();

    //installSoundFIFO(); // Don't setup sound FIFO
    installSystemFIFO(); // Sleep mode, storage, firmware...

    // This sets a callback that is called when the power button in a DSi
    // console is pressed. It has no effect in a DS.
    setPowerButtonCB(power_button_callback);

    // Read current date from the RTC and setup an interrupt to update the time
    // regularly. The interrupt simply adds one second every time, it doesn't
    // read the date. Reading the RTC is very slow, so it's a bad idea to do it
    // frequently.
    initClockIRQTimer(3);

    // Now that the FIFO is setup we can start sending input data to the ARM9.
    irqSet(IRQ_VBLANK, vblank_handler);
    irqEnable(IRQ_VBLANK);

    while (!consoleIsSetup())
        swiWaitForVBlank();

    consolePuts("ARM7 ready!\n");

    uint16_t keys_old = 0;

    while (!exit_loop)
    {
        uint16_t keys_pressed = ~REG_KEYINPUT;
        uint16_t keys_down = keys_pressed & ~keys_old;
        keys_old = keys_pressed;

        const uint16_t key_mask = KEY_SELECT | KEY_START | KEY_L | KEY_R;
        if ((keys_pressed & key_mask) == key_mask)
            exit_loop = true;

        swiWaitForVBlank();

        if (keys_down & KEY_R)
            check_value32_packets();
        if (keys_down & KEY_A)
            check_address_packets();
        if (keys_down & KEY_B)
            check_data_packets();
    }

    return 0;
}
