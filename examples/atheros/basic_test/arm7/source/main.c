// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023
// SPDX-FileContributor: PoroCYon, 2026

#include <stdio.h>

#include <dsiwifi7_bmi.h>
#include <nds.h>

#include "xtensa_bin.h"

static uint32_t bmi_ver = 0;

static uint32_t xt_base, xt_database;

uint8_t textdata_bin[] = { 1, 2, 3, 4, 5, 6, 7 };

static unsigned int xtensa_initialize(void)
{
    int r = ath6k_init_hw_to_bmi();
    if (r != 0)
        return 0;

    //consolePrintf("BMI init: %d\n", r);
    //consoleFlush();

    // 0x20000188: ar6002 (dwm-w015)
    // 0x23000024: ar6013 (dwm-w024)
    // 0x2300006F: ar6014 (dwm-w028)
    bmi_ver = ath6k_bmi_get_version();
    //consolePrintf("wifi module ver: 0x%x\n", (unsigned int)bmi_ver);
    //consoleFlush();

    if (bmi_ver == 0x20000188)
    {
        xt_base = 0x504000;
        xt_database = xt_base - 0x100;
    }
    else
    {
        xt_base = 0x53c000;
        xt_database = xt_base + 0x3000;
    }

    ath6k_bmi_write_memory(xt_base, xtensa_bin, sizeof(xtensa_bin));

    // Return the version of BMI on success
    return bmi_ver;
}

static void fifo_handler_value32(u32 v, void *ud)
{
    // This is a very basic example of how to run a custom program in the
    // Xtensa. It loads our binary, passes a pointer to it, and waits for it to
    // return a value.
    //
    // In our buffer we only have a 32-bit value. The _start() function will
    // receive a pointer to this buffer, so it can read the 32-bit value. You
    // can write more data if you want, of course, but this is a very basic
    // example.

    (void)ud;

    int value = v;

    uint8_t *buffer = (uint8_t *)&value;
    size_t len = sizeof(value);

    ath6k_bmi_write_memory(xt_database, (const uint8_t *)buffer, len);

    int r = ath6k_bmi_execute(xt_base + 0x400000, xt_database);

    // The CPU can also return data by writing it to the same buffer that was
    // used for the initial transfer. This example doesn't use this system.
    ath6k_bmi_read_memory(xt_database, len, (uint8_t*)buffer);

    fifoSendValue32(FIFO_USER_01, value);
    fifoSendValue32(FIFO_USER_02, r);
}

volatile bool exit_loop = false;

void power_button_callback(void)
{
    exit_loop = true;
}

void vblank_handler(void)
{
    inputGetAndSend();
}

int main(int argc, char **argv)
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

    // ARM7 needs:
    // * NDMA
    // * WIFI_SDIO
    // * I2C
    //uint32_t need = SCFG_EXT_NDMA | SCFG_EXT_WIFI_SDIO | SCFG_EXT_GPIO | SCFG_EXT_I2C | SCFG_EXT_SNDEXCNT;
    //uint32_t ok_extra = ((1<<14) - 1) | SCFG_EXT_SDMMC;

    // Init
    unsigned int val = xtensa_initialize();
    fifoSendValue32(FIFO_USER_02, val);

    // Only start handling messages if the initialization went ok
    if (val != 0)
        fifoSetValue32Handler(FIFO_USER_01, fifo_handler_value32, NULL);

    while (!exit_loop)
    {
        // The console is setup to redirect stderr to the no$gba debug console
        // by default.
        //fprintf(stderr, "ARM7 %%frame%%\n");

        const uint16_t key_mask = KEY_SELECT | KEY_START | KEY_L | KEY_R;
        uint16_t keys_pressed = ~REG_KEYINPUT;

        if ((keys_pressed & key_mask) == key_mask)
            exit_loop = true;

        swiWaitForVBlank();
    }

    return 0;
}
