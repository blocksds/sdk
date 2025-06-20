// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023-2024

#include <stdbool.h>
#include <stdio.h>

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

volatile bool irq_handled = false;

void timer0_handler(void)
{
    irq_handled = true;
}

void halt(void)
{
    swiHalt();
}

void run_test(void)
{
    consolePrintf("ARM7 Testing... ");
    consoleFlush();

    // 300 ticks per second
    timerStart(0, ClockDivider_1024, TIMER_FREQ_1024(300), timer0_handler);

    irq_handled = false;

    halt();
    halt();
    halt();

    timerStop(0);

    consolePrintf("%s\n", irq_handled ? "Handled" : "Not handled");
    consoleFlush();

    // Go back to normal
    REG_IME = 1;
    setCPSR(getCPSR() & ~CPSR_FLAG_IRQ_DIS);
}

void FIFO_UserValue32Handler(u32 value, void *userdata)
{
    if (value == 0)
    {
        REG_IME = 1;
        setCPSR(getCPSR() & ~CPSR_FLAG_IRQ_DIS);
        run_test();
    }

    if (value == 1)
    {
        REG_IME = 1;
        setCPSR(getCPSR() | CPSR_FLAG_IRQ_DIS);
        run_test();
    }

    if (value == 2)
    {
        REG_IME = 0;
        setCPSR(getCPSR() & ~CPSR_FLAG_IRQ_DIS);
        run_test();
    }

    if (value == 3)
    {
        REG_IME = 0;
        setCPSR(getCPSR() | CPSR_FLAG_IRQ_DIS);
        run_test();
    }
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
    fifoInit();

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
