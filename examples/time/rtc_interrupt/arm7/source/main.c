// SPDX-License-Identifier: Zlib
//
// Copyright (C) 2005 Michael Noland (joat)
// Copyright (C) 2005 Jason Rogers (Dovoto)
// Copyright (C) 2005-2015 Dave Murphy (WinterMute)
// Copyright (C) 2023 Antonio Niño Díaz

// ARM7 core that uses the RTC interrupt instead of using a timer interrupt to
// update the time every second.

#include <dswifi7.h>
#include <nds.h>
#include <maxmod7.h>

volatile bool exit_loop = false;

void power_button_callback(void)
{
    exit_loop = true;
}

void vblank_handler(void)
{
    inputGetAndSend();
    Wifi_Update();
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

    installWifiFIFO();
    installSoundFIFO();
    installSystemFIFO(); // Sleep mode, storage, firmware...

    // Initialize Maxmod. It uses timer 0 internally.
    mmInstall(FIFO_MAXMOD);

    // This sets a callback that is called when the power button in a DSi
    // console is pressed. It has no effect in a DS.
    setPowerButtonCB(power_button_callback);

    // Read current date from the RTC and setup an interrupt to update the time
    // regularly. The interrupt simply adds one second every time, it doesn't
    // read the date. Reading the RTC is very slow, so it's a bad idea to do it
    // frequently. This function only sets up the RTC interrupt, it doesn't
    // enable it.
    //
    // Also, note that it has been deprecated. The pragmas here are used to
    // silence the warning that GCC generates when using the function.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    initClockIRQ();
#pragma GCC diagnostic pop

    // Now that the FIFO is setup we can start sending input data to the ARM9.
    irqSet(IRQ_VBLANK, vblank_handler);
    irqEnable(IRQ_VBLANK | IRQ_RTC); // Enable the RTC interrupt

    while (!exit_loop)
    {
        const uint16_t key_mask = KEY_SELECT | KEY_START | KEY_L | KEY_R;
        uint16_t keys_pressed = ~REG_KEYINPUT;

        if ((keys_pressed & key_mask) == key_mask)
            exit_loop = true;

        swiWaitForVBlank();
    }

    return 0;
}
