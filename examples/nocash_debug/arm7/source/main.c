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

    while (!exitflag)
    {
        // The console is setup to redirect stderr to the no$gba debug console
        // by default.
        fprintf(stderr,"ARM7 %%frame%%\n");

        const uint16_t key_mask = KEY_SELECT | KEY_START | KEY_L | KEY_R;
        uint16_t keys_pressed = ~REG_KEYINPUT;

        if ((keys_pressed & key_mask) == key_mask)
            exitflag = true;

        swiWaitForVBlank();
    }

    return 0;
}
