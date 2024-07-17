// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

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

void fifo_handler_value32(u32 command, void *userdata)
{
    uint32_t channel = (uint32_t)userdata;

    fifoSendValue32(channel, command + 0x10);
}

void fifo_handler_address(void *address, void *userdata)
{
    uint32_t channel = (uint32_t)userdata;

    uint32_t value = (uint32_t)address;
    fifoSendAddress(channel, (void *)(value + 0x10));
}

typedef struct {
    uint8_t channel;
    uint32_t value;
} datamsg;

void fifo_handler_datamsg(int num_bytes, void *userdata)
{
    uint32_t expected_channel = (uint32_t)userdata;

    datamsg msg;

    fifoGetDatamsg(expected_channel, sizeof(msg), (void *)&msg);

    msg.value++;

    fifoSendDatamsg(expected_channel, sizeof(msg), (void *)&msg);
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

    for (uint32_t ch = FIFO_USER_01; ch <= FIFO_USER_08; ch++)
    {
        fifoSetValue32Handler(ch, fifo_handler_value32, (void *)ch);
        fifoSetAddressHandler(ch, fifo_handler_address, (void *)ch);
        fifoSetDatamsgHandler(ch, fifo_handler_datamsg, (void *)ch);
    }

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
