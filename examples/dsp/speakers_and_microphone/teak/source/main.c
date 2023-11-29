// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <teak/teak.h>

// All memory from 0x1000 to 0x7FFF and 0x9000 to 0xFFFF is used as buffer for
// the recording.
const uint16_t BUFFER_BASE_1 = 0x1000;
const uint16_t BUFFER_END_1 = 0x8000;
const uint16_t BUFFER_BASE_2 = 0x9000;
const uint16_t BUFFER_END_2 = 0x0000; // 0x10000

// Address that is going to be accessed next
uint16_t BUFFER_CURRENT;

#define CMD_NONE    0
#define CMD_RECORD  1
#define CMD_PLAY    2

// Current command
uint16_t command = CMD_NONE;

// This function is defined as a weak symbol in the crt0, and it can be
// overwritten by the user.
void irqHandlerInt1(void)
{
    if (command == CMD_RECORD) // Microphone
    {
        while (btdmpReceiveFifoEmpty(0));
        uint16_t left = REG_BTDMP_RECEIVE_FIFO_DATA(0);
        *(volatile uint16_t *)BUFFER_CURRENT = left;
        BUFFER_CURRENT++;

        while (btdmpReceiveFifoEmpty(0));
        uint16_t right = REG_BTDMP_RECEIVE_FIFO_DATA(0);
        *(volatile uint16_t *)BUFFER_CURRENT = right;
        BUFFER_CURRENT++;

        if (BUFFER_CURRENT == BUFFER_END_1)
        {
            BUFFER_CURRENT = BUFFER_BASE_2;
        }
        else if (BUFFER_CURRENT == BUFFER_END_2)
        {
            // Stop when the end of the buffer has been reached
            btdmpDisableInput(0);
            command = CMD_NONE;
            cpuDisableIrqs();
        }
    }
    else if (command == CMD_PLAY) // Speakers
    {
        uint16_t left = *(volatile uint16_t *)BUFFER_CURRENT;
        BUFFER_CURRENT++;
        uint16_t right = *(volatile uint16_t *)BUFFER_CURRENT;
        BUFFER_CURRENT++;

        while (btdmpTransmitFifoFull(0));
        REG_BTDMP_TRANSMIT_FIFO_DATA(0) = left;
        while (btdmpTransmitFifoFull(0));
        REG_BTDMP_TRANSMIT_FIFO_DATA(0) = right;

        if (BUFFER_CURRENT == BUFFER_END_1)
        {
            BUFFER_CURRENT = BUFFER_BASE_2;
        }
        else if (BUFFER_CURRENT == BUFFER_END_2)
        {
            // Stop when the end of the buffer has been reached
            btdmpDisableOutput(0);
            command = CMD_NONE;
            cpuDisableIrqs();
        }
    }

    REG_ICU_IRQ_ACK = ICU_IRQ_MASK_BTDMP0;
}

int main(void)
{
    teakInit();

    uint16_t old_command_count = 0;

    cpuEnableInt1();

    while (1)
    {
        uint16_t data0 = apbpReceiveData(0); // Heartbeat
        uint16_t data1 = apbpReceiveData(1);
        uint16_t data2 = apbpReceiveData(2);

        uint16_t new_command = data1;
        uint16_t command_count = data2;

        if (command_count != old_command_count)
        {
            old_command_count = command_count;

            command = new_command;

            cpuDisableIrqs();
            btdmpDisableOutput(0);
            btdmpDisableInput(0);

            // Restart buffer
            BUFFER_CURRENT = BUFFER_BASE_1;

            if (command == CMD_RECORD) // Microphone
            {
                // Enable BTDMP channel 0 and make it trigger interrupts in CPU
                // interrupt 1.
                btdmpSetupInputMicrophone(0, 1);
                cpuEnableIrqs();
            }
            else if (command == CMD_PLAY) // Speakers
            {
                // Enable BTDMP channel 0 and make it trigger interrupts in CPU
                // interrupt 1.
                btdmpSetupOutputSpeakers(0, 1);
                cpuEnableIrqs();
            }
        }

        apbpSendData(0, data0);
        apbpSendData(1, command);
        apbpSendData(2, BUFFER_CURRENT);
    }

    return 0;
}
