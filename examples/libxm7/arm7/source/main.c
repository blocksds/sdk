// SPDX-License-Identifier: Zlib
//
// Copyright (C) 2005 Michael Noland (joat)
// Copyright (C) 2005 Jason Rogers (Dovoto)
// Copyright (C) 2005-2015 Dave Murphy (WinterMute)
// Copyright (C) 2023 Antonio Niño Díaz

#include <dswifi7.h>
#include <libxm7.h>
#include <nds.h>

// Assign FIFO_USER_07 channel to libxm7
#define FIFO_XM7    (FIFO_USER_07)

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

void XM7_Value32Handler(u32 command, void *userdata)
{
    XM7_ModuleManager_Type *module = (XM7_ModuleManager_Type *)command;

    if (module == NULL)
        XM7_StopModule();
    else
        XM7_PlayModule(module);
}

int main(int argc, char *argv[])
{
    // Initialize sound hardware
    enableSound();

    // Read user information from the firmware (name, birthday, etc)
    readUserSettings();

    // Stop LED blinking
    ledBlink(0);

    // Using the calibration values read from the firmware with
    // readUserSettings(), calculate some internal values to convert raw
    // coordinates into screen coordinates.
    touchInit();

    irqInit();
    irqSet(IRQ_VBLANK, vblank_handler);

    fifoInit();

    installWifiFIFO();
    installSoundFIFO();
    installSystemFIFO(); // Sleep mode, storage, firmware...

    // This sets a callback that is called when the power button in a DSi
    // console is pressed. It has no effect in a DS.
    setPowerButtonCB(power_button_callback);

    // Read current date from the RTC and setup an interrupt to update the time
    // regularly. The interrupt simply adds one second every time, it doesn't
    // read the date. Reading the RTC is very slow, so it's a bad idea to do it
    // frequently.
    initClockIRQTimer(3);

    irqEnable(IRQ_VBLANK);

    // Initialize libxm7. It uses timer 1 internally.
    XM7_Initialize();
    // Setup the FIFO handler for libXM7
    fifoSetValue32Handler(FIFO_XM7, XM7_Value32Handler, 0);

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
