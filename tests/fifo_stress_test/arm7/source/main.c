// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <nds.h>

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

    for (uint32_t ch = FIFO_USER_01; ch <= FIFO_USER_08; ch++)
    {
        fifoSetValue32Handler(ch, fifo_handler_value32, (void *)ch);
        fifoSetAddressHandler(ch, fifo_handler_address, (void *)ch);
        fifoSetDatamsgHandler(ch, fifo_handler_datamsg, (void *)ch);
    }

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
