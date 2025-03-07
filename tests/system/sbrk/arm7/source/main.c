// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023-2025

#include <stdio.h>
#include <stdlib.h>

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

// Symbols defined by the linker
extern char __end__[];
const uintptr_t end = (uintptr_t)__end__;

extern char __twl_end__[];
const uintptr_t twl_end = (uintptr_t)__twl_end__;

void FIFO_UserValue32Handler(u32 address, void *userdata)
{
    consolePrintf("DS Mode\n");
    consolePrintf("\n");
    unsigned int ds_heap_start = (unsigned int)end;
    unsigned int ds_heap_end = (unsigned int)(0x03800000 + 64 * 1024);
    consolePrintf("__end__:       0x%x\n", ds_heap_start);
    consolePrintf("iwram end:     0x%x\n", ds_heap_end);
    consolePrintf("heap size:     %u B\n", ds_heap_end - ds_heap_start);
    consolePrintf("\n");

    consolePrintf("DSi Mode\n");
    consolePrintf("\n");
    unsigned int dsi_heap_start = (unsigned int)twl_end;
    unsigned int dsi_heap_end = (unsigned int)(0x03000000 + 256 * 1024);
    consolePrintf("__twl_end__:   0x%x\n", dsi_heap_start);
    consolePrintf("twl_iwram end: 0x%x\n", dsi_heap_end);
    consolePrintf("heap size:     %u B\n", dsi_heap_end - dsi_heap_start);
    consolePrintf("\n");

    consolePrintf("Tests\n");
    consolePrintf("\n");
    consolePrintf("malloc(1 KB):   0x%x\n", (unsigned int)malloc(1024));
    consolePrintf("malloc(16 KB):  0x%x\n", (unsigned int)malloc(16 * 1024));
    consolePrintf("malloc(32 KB):  0x%x\n", (unsigned int)malloc(32 * 1024));
    consolePrintf("malloc(32 KB):  0x%x\n", (unsigned int)malloc(32 * 1024));
    consolePrintf("malloc(64 KB):  0x%x\n", (unsigned int)malloc(64 * 1024));
    consolePrintf("malloc(64 KB):  0x%x\n", (unsigned int)malloc(64 * 1024));
    consolePrintf("malloc(128 KB): 0x%x\n", (unsigned int)malloc(128 * 1024));

    consoleFlush();
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
    initClockIRQTimer(3);

    // Now that the FIFO is setup we can start sending input data to the ARM9.
    irqSet(IRQ_VBLANK, vblank_handler);
    irqEnable(IRQ_VBLANK);

    // Setup the FIFO handler
    fifoSetValue32Handler(FIFO_USER_01, FIFO_UserValue32Handler, 0);

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
