// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <stdio.h>

#include <nds.h>

void vblank_handler(void)
{
    inputGetAndSend();
}

volatile bool exitflag = false;

void power_button_callback(void)
{
    exitflag = true;
}

void FIFO_UserAddressHandler(void *address, void *userdata)
{
    if (address == NULL)
    {
        fifoSendValue32(FIFO_USER_01, 0xFFFFFFFF);
        return;
    }

    // Take the first 32 bits of the buffer and treat it as the size in words.
    // Then, add the remaining words and return the result to the ARM9.

    uint32_t *buffer = address;
    uint32_t size = *buffer++;
    uint32_t sum = 0;

    while (size-- > 0)
        sum += *buffer++;

    fifoSendValue32(FIFO_USER_01, sum);
}

int main(int argc, char **argv)
{
    REG_SOUNDCNT &= ~SOUND_ENABLE;
    powerOff(POWER_SOUND);

    readUserSettings();
    ledBlink(0);

    irqInit();

    fifoInit();
    installSystemFIFO();

    // Setup the FIFO handler
    fifoSetAddressHandler(FIFO_USER_01, FIFO_UserAddressHandler, 0);

    irqSet(IRQ_VBLANK, vblank_handler);
    irqEnable(IRQ_VBLANK);

    setPowerButtonCB(power_button_callback);

    while (!exitflag)
    {
        const uint16_t key_mask = KEY_SELECT | KEY_START | KEY_L | KEY_R;
        uint16_t keys_pressed = ~REG_KEYINPUT;

        if ((keys_pressed & key_mask) == key_mask)
            exitflag = true;

        swiWaitForVBlank();
    }

    return 0;
}
