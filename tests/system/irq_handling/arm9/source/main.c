// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

// This test lets you check different combinations of IME and CPSR.I and
// experiment if the ARM9/ARM7 can exit halt state from it. It also checks if
// the user interrupt handler is actually called or not.
//
// ARM9
//         | CPSR.I=0                | CPSR.I=1
// --------+-------------------------+------------
// IME = 1 | Exits halt, handles IRQ | Hangs
// --------+-------------------------+------------
// IME = 0 | Exits halt              | Hangs
//
// ARM7
//         | CPSR.I=0                | CPSR.I=1
// --------+-------------------------+------------
// IME = 1 | Exits halt, handles IRQ | Exits halt
// --------+-------------------------+------------
// IME = 0 | Exits halt              | Exits halt

#include <stdbool.h>
#include <stdio.h>

#include <nds.h>

volatile bool irq_handled = false;

void timer0_handler(void)
{
    irq_handled = true;
}

void halt(void)
{
    CP15_WaitForInterrupt();
}

void run_test(void)
{
    printf("ARM9 Testing... ");

    // 300 ticks per second
    timerStart(0, ClockDivider_1024, TIMER_FREQ_1024(300), timer0_handler);

    irq_handled = false;

    halt();
    halt();
    halt();

    timerStop(0);

    printf("%s\n", irq_handled ? "Handled" : "Not handled");

    // Go back to normal
    REG_IME = 1;
    setCPSR(getCPSR() & ~CPSR_FLAG_IRQ_DIS);
}

int main(int argc, char **argv)
{
    defaultExceptionHandler();

    PrintConsole topScreen;
    PrintConsole bottomScreen;

    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(&topScreen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    consoleInit(&bottomScreen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);

    consoleSelect(&topScreen);

    printf("ARM9\n");
    printf("====\n");
    printf("A:     IME=1, CPSR.I=0\n");
    printf("B:     IME=1, CPSR.I=1\n");
    printf("X:     IME=0, CPSR.I=0\n");
    printf("Y:     IME=0, CPSR.I=1\n");
    printf("\n");
    printf("ARM7\n");
    printf("====\n");
    printf("RIGHT: IME=1, CPSR.I=0\n");
    printf("DOWN:  IME=1, CPSR.I=1\n");
    printf("UP:    IME=0, CPSR.I=0\n");
    printf("LEFT:  IME=0, CPSR.I=1\n");
    printf("\n");
    printf("\n");
    printf("START: Exit\n");

    consoleSelect(&bottomScreen);
    consoleArm7Setup(&bottomScreen, 1024);

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();
        uint16_t keys = keysDown();

        if (keys & KEY_START)
            break;

        if (keys & KEY_A)
        {
            REG_IME = 1;
            setCPSR(getCPSR() & ~CPSR_FLAG_IRQ_DIS);
            run_test();
        }
        if (keys & KEY_B)
        {
            REG_IME = 1;
            setCPSR(getCPSR() | CPSR_FLAG_IRQ_DIS);
            run_test();
        }
        if (keys & KEY_X)
        {
            REG_IME = 0;
            setCPSR(getCPSR() & ~CPSR_FLAG_IRQ_DIS);
            run_test();
        }
        if (keys & KEY_Y)
        {
            REG_IME = 0;
            setCPSR(getCPSR() | CPSR_FLAG_IRQ_DIS);
            run_test();
        }

        if (keys & KEY_RIGHT)
        {
            fifoSendValue32(FIFO_USER_01, 0);
        }
        if (keys & KEY_DOWN)
        {
            fifoSendValue32(FIFO_USER_01, 1);
        }
        if (keys & KEY_UP)
        {
            fifoSendValue32(FIFO_USER_01, 2);
        }
        if (keys & KEY_LEFT)
        {
            fifoSendValue32(FIFO_USER_01, 3);
        }
    }

    return 0;
}
