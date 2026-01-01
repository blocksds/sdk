// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023-2026

// This tries to cause an unhandled C++ exception, which should be managed by
// the function passed to std::set_terminate().

#include <stdlib.h>

#include <exception>
#include <vector>

#include <nds.h>

volatile bool exit_loop = false;

void power_button_callback(void)
{
    exit_loop = true;
}

void vblank_handler(void)
{
    inputGetAndSend();
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

    installSoundFIFO();
    installSystemFIFO(); // Sleep mode, storage, firmware...

    // This sets a callback that is called when the power button in a DSi
    // console is pressed. It has no effect in a DS.
    setPowerButtonCB(power_button_callback);

    // Read current date from the RTC and setup an interrupt to update the time
    // regularly. The interrupt simply adds one second every time, it doesn't
    // read the date. Reading the RTC is very slow, so it's a bad idea to do it
    // frequently.
    initClockIRQTimer(LIBNDS_DEFAULT_TIMER_RTC);

    // Now that the FIFO is setup we can start sending input data to the ARM9.
    irqSet(IRQ_VBLANK, vblank_handler);
    irqEnable(IRQ_VBLANK);

    // Wait until the ARM7 console is setup before printing to it
    while (!consoleIsSetup())
        swiWaitForVBlank();

    std::set_terminate([]()
    {
        consolePuts("Unhandled exception!\nExitting program...");
        consoleFlush();

        for (int i = 0; i < 5; i++)
        {
            consolePuts(".");
            consoleFlush();
            for (int j = 0; j < 60; j++)
                swiWaitForVBlank();
        }

        exit(0);
    });

    consolePuts("===== ARM7 =====\n");
    consolePuts("B: Trigger C++ exception\n\n");

    consoleFlush();

    while (!exit_loop)
    {
        const uint16_t key_mask = KEY_SELECT;
        uint16_t keys_pressed = ~REG_KEYINPUT;

        if (keys_pressed & KEY_B)
        {
            consolePuts("Triggering exception...\n");
            consoleFlush();

            std::vector<int> vec(1024 * 1024, 100);

            consolePuts("This shouldn't be reached.\n");
            consoleFlush();
        }

        if ((keys_pressed & key_mask) == key_mask)
            exit_loop = true;

        swiWaitForVBlank();
    }

    return 0;
}
