// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <nds.h>

void fifo_value32_handler(u32 command, void *userdata)
{
    fifoSendValue32((uint32_t)userdata, command + 1);
}

void vblank_handler(void)
{
    inputGetAndSend();
}

volatile bool exitflag = false;

void power_button_callback(void)
{
    exitflag = true;
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

    irqSet(IRQ_VBLANK, vblank_handler);
    irqEnable(IRQ_VBLANK);

    setPowerButtonCB(power_button_callback);

    for (uint32_t ch = FIFO_USER_01; ch < FIFO_USER_08; ch++)
        fifoSetValue32Handler(ch, fifo_value32_handler, (void *)ch);

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
