// SPDX-License-Identifier: Zlib
//
// Copyright (C) 2005 Michael Noland (joat)
// Copyright (C) 2005 Jason Rogers (Dovoto)
// Copyright (C) 2005-2015 Dave Murphy (WinterMute)

// Default ARM7 core

#include <nds.h>
#include <dswifi7.h>
#include <maxmod7.h>

void VblankHandler(void)
{
    Wifi_Update();
}

void VcountHandler(void)
{
    inputGetAndSend();
}

volatile bool exitflag = false;

void powerButtonCB(void)
{
    exitflag = true;
}

int main(int argc, char **argv)
{
    // Clear sound registers
    dmaFillWords(0, (void*)0x04000400, 0x100);

    REG_SOUNDCNT |= SOUND_ENABLE;
    writePowerManagement(PM_CONTROL_REG,
        (readPowerManagement(PM_CONTROL_REG) & ~PM_SOUND_MUTE) | PM_SOUND_AMP);
    powerOn(POWER_SOUND);

    readUserSettings();
    ledBlink(0);

    irqInit();
    // Start the RTC tracking IRQ
    initClockIRQ();
    fifoInit();
    touchInit();

    mmInstall(FIFO_MAXMOD);

    SetYtrigger(80);

    installWifiFIFO();
    installSoundFIFO();

    installSystemFIFO();

    if (isDSiMode())
        installCameraFIFO();

    irqSet(IRQ_VCOUNT, VcountHandler);
    irqSet(IRQ_VBLANK, VblankHandler);

    irqEnable(IRQ_VBLANK | IRQ_VCOUNT | IRQ_NETWORK);

    setPowerButtonCB(powerButtonCB);

    // Keep the ARM7 mostly idle
    while (!exitflag)
    {
        if (0 == (REG_KEYINPUT & (KEY_SELECT | KEY_START | KEY_L | KEY_R)))
            exitflag = true;

        swiWaitForVBlank();
    }

    return 0;
}
