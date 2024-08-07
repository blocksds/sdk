// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

// This test can be used to verify which permissions homebrew applications have
// with the loader that has been used to run it.

#include <stdio.h>

#include <nds.h>

static PrintConsole topScreen;
static PrintConsole bottomScreen;

void print_scfg_a7rom_a9rom(uint8_t scfg_a7rom, uint8_t scfg_a9rom)
{
    consoleSelect(&topScreen);

    consoleClear();

    printf("SCFG_A9ROM : 0x%02X\n", scfg_a9rom);
    printf("-----------------\n");
    printf("0. Access to all BIOS:       %d\n", scfg_a9rom & BIT(0) ? 0 : 1);
    printf("1. DSi BIOS:                 %d\n", scfg_a9rom & BIT(1) ? 0 : 1);
    printf("\n");
    printf("\n");

    consoleSelect(&bottomScreen);

    consoleClear();

    printf("SCFG_A7ROM : 0x%02X\n", scfg_a7rom);
    printf("-----------------\n");
    printf("0. Access to all BIOS:       %d\n", scfg_a7rom & BIT(0) ? 0 : 1);
    printf("1. DSi BIOS:                 %d\n", scfg_a7rom & BIT(1) ? 0 : 1);
    printf("2. Access to console ID:     %d\n", scfg_a7rom & BIT(2) ? 0 : 1);

    consoleSetCursor(NULL, 0, 21);
    printf("Press A to continue\n");
    printf("\n");
    printf("Press START to exit");
}

void print_scfg_ext7(uint32_t scfg_ext7)
{
    consoleSelect(&topScreen);

    consoleClear();

    printf("SCFG_EXT7 : 0x%08lX\n", scfg_ext7);
    printf("----------------------\n");
    printf("0.  Rev. ARM7 DMA Circuit:   %d\n", scfg_ext7 & BIT(0) ? 1 : 0);
    printf("1.  Rev. Sound DMA:          %d\n", scfg_ext7 & BIT(1) ? 1 : 0);
    printf("2.  Rev. Sound:              %d\n", scfg_ext7 & BIT(2) ? 1 : 0);
    printf("3.  Undocumented:            %d\n", scfg_ext7 & BIT(3) ? 1 : 0);
    printf("4.  Undocumented:            %d\n", scfg_ext7 & BIT(4) ? 1 : 0);
    printf("5.  Undocumented:            %d\n", scfg_ext7 & BIT(5) ? 1 : 0);
    printf("6.  Undocumented:            %d\n", scfg_ext7 & BIT(6) ? 1 : 0);
    printf("7.  Rev. Card Interface:     %d\n", scfg_ext7 & BIT(7) ? 1 : 0);
    printf("8.  Ext. ARM7 IRQs:          %d\n", scfg_ext7 & BIT(8) ? 1 : 0);
    printf("9.  Ext. SPI Clock:          %d\n", scfg_ext7 & BIT(9) ? 1 : 0);
    printf("10. Ext. Sound DMA:          %d\n", scfg_ext7 & BIT(10) ? 1 : 0);
    printf("11. Undocumented:            %d\n", scfg_ext7 & BIT(11) ? 1 : 0);
    printf("12. Ext. LCD Circuit:        %d\n", scfg_ext7 & BIT(12) ? 1 : 0);
    printf("13. Ext. VRAM Access:        %d\n", scfg_ext7 & BIT(13) ? 1 : 0);

    unsigned int ram_sizes[4] = {4, 4, 16, 32};
    printf("14. Main RAM Size:       %2u MB\n", ram_sizes[(scfg_ext7 >> 14) & 3]);

    printf("16. Access New DMA:          %d\n", scfg_ext7 & BIT(16) ? 1 : 0);
    printf("17. Access AES Unit:         %d\n", scfg_ext7 & BIT(17) ? 1 : 0);
    printf("18. Access SD/MMC:           %d\n", scfg_ext7 & BIT(18) ? 1 : 0);
    printf("19. Access SDIO Wifi:        %d\n", scfg_ext7 & BIT(19) ? 1 : 0);
    printf("20. Access Microphone:       %d\n", scfg_ext7 & BIT(20) ? 1 : 0);

    consoleSelect(&bottomScreen);

    consoleClear();

    printf("21. Access SNDEXCNT:         %d\n", scfg_ext7 & BIT(21) ? 1 : 0);
    printf("22. Access I2C:              %d\n", scfg_ext7 & BIT(22) ? 1 : 0);
    printf("23. Access GPIO:             %d\n", scfg_ext7 & BIT(23) ? 1 : 0);
    printf("24. Access 2nd NDS Slot:     %d\n", scfg_ext7 & BIT(24) ? 1 : 0);
    printf("25. Access NWRAM:            %d\n", scfg_ext7 & BIT(25) ? 1 : 0);
    printf("26. Undocumented:            %d\n", scfg_ext7 & BIT(26) ? 1 : 0);
    printf("27. Undocumented:            %d\n", scfg_ext7 & BIT(27) ? 1 : 0);
    printf("28. Undocumented:            %d\n", scfg_ext7 & BIT(28) ? 1 : 0);
    printf("29. Undocumented:            %d\n", scfg_ext7 & BIT(29) ? 1 : 0);
    printf("30. Undocumented:            %d\n", scfg_ext7 & BIT(30) ? 1 : 0);
    printf("31. Access SCFG/MBK:         %d\n", scfg_ext7 & BIT(31) ? 1 : 0);

    consoleSetCursor(NULL, 0, 21);
    printf("Press A to continue\n");
    printf("\n");
    printf("Press START to exit");
}

void print_scfg_ext9(uint32_t scfg_ext9)
{
    consoleSelect(&topScreen);

    consoleClear();

    printf("SCFG_EXT9 : 0x%08lX\n", scfg_ext9);
    printf("----------------------\n");
    printf("0.  Rev. ARM9 DMA Circuit:   %d\n", scfg_ext9 & BIT(0) ? 1 : 0);
    printf("1.  Rev. Geometry Circuit:   %d\n", scfg_ext9 & BIT(1) ? 1 : 0);
    printf("2.  Rev. Renderer Circuit:   %d\n", scfg_ext9 & BIT(2) ? 1 : 0);
    printf("3.  Rev. 2D Engine Circuit:  %d\n", scfg_ext9 & BIT(3) ? 1 : 0);
    printf("4.  Rev. Divider Circuit:    %d\n", scfg_ext9 & BIT(4) ? 1 : 0);
    printf("5.  Undocumented:            %d\n", scfg_ext9 & BIT(5) ? 1 : 0);
    printf("6.  Undocumented:            %d\n", scfg_ext9 & BIT(6) ? 1 : 0);
    printf("7.  Rev. Card Interface:     %d\n", scfg_ext9 & BIT(7) ? 1 : 0);
    printf("8.  Ext. ARM9 IRQs:          %d\n", scfg_ext9 & BIT(8) ? 1 : 0);
    printf("9.  Undocumented:            %d\n", scfg_ext9 & BIT(9) ? 1 : 0);
    printf("10. Undocumented:            %d\n", scfg_ext9 & BIT(10) ? 1 : 0);
    printf("11. Undocumented:            %d\n", scfg_ext9 & BIT(11) ? 1 : 0);
    printf("12. Ext. LCD Circuit:        %d\n", scfg_ext9 & BIT(12) ? 1 : 0);
    printf("13. Ext. VRAM Access:        %d\n", scfg_ext9 & BIT(13) ? 1 : 0);

    unsigned int ram_sizes[4] = {4, 4, 16, 32};
    printf("14. Main RAM Size:       %2u MB\n", ram_sizes[(scfg_ext9 >> 14) & 3]);

    printf("16. Access New DMA:          %d\n", scfg_ext9 & BIT(16) ? 1 : 0);
    printf("17. Access Camera:           %d\n", scfg_ext9 & BIT(17) ? 1 : 0);
    printf("18. Access Teak DSP:         %d\n", scfg_ext9 & BIT(18) ? 1 : 0);
    printf("19. Undocumented:            %d\n", scfg_ext9 & BIT(19) ? 1 : 0);
    printf("20. Undocumented:            %d\n", scfg_ext9 & BIT(20) ? 1 : 0);

    consoleSelect(&bottomScreen);

    consoleClear();

    printf("21. Undocumented:            %d\n", scfg_ext9 & BIT(21) ? 1 : 0);
    printf("22. Undocumented:            %d\n", scfg_ext9 & BIT(22) ? 1 : 0);
    printf("23. Undocumented:            %d\n", scfg_ext9 & BIT(23) ? 1 : 0);
    printf("24. Access 2nd NDS Slot:     %d\n", scfg_ext9 & BIT(24) ? 1 : 0);
    printf("25. Access NWRAM:            %d\n", scfg_ext9 & BIT(25) ? 1 : 0);
    printf("26. Undocumented:            %d\n", scfg_ext9 & BIT(26) ? 1 : 0);
    printf("27. Undocumented:            %d\n", scfg_ext9 & BIT(27) ? 1 : 0);
    printf("28. Undocumented:            %d\n", scfg_ext9 & BIT(28) ? 1 : 0);
    printf("29. Undocumented:            %d\n", scfg_ext9 & BIT(29) ? 1 : 0);
    printf("30. Undocumented:            %d\n", scfg_ext9 & BIT(30) ? 1 : 0);
    printf("31. Access SCFG/MBK:         %d\n", scfg_ext9 & BIT(31) ? 1 : 0);

    consoleSetCursor(NULL, 0, 23);
    printf("Press A to continue\n");
    printf("\n");
    printf("Press START to exit");
}

int main(int argc, char **argv)
{
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(&topScreen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    consoleInit(&bottomScreen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);

    swiWaitForVBlank();
    swiWaitForVBlank();

    uint8_t scfg_a9rom = REG_SCFG_ROM;

    fifoSendValue32(FIFO_USER_01, 0);
    fifoWaitValue32(FIFO_USER_01);
    uint8_t scfg_a7rom = fifoGetValue32(FIFO_USER_01);

    fifoSendValue32(FIFO_USER_01, 1);
    fifoWaitValue32(FIFO_USER_01);
    uint32_t scfg_ext7 = fifoGetValue32(FIFO_USER_01);

    uint32_t scfg_ext9 = REG_SCFG_EXT;

    int page = 0;

    while (1)
    {
        switch (page)
        {
            case 0:
                print_scfg_a7rom_a9rom(scfg_a7rom, scfg_a9rom);
                break;
            case 1:
                print_scfg_ext7(scfg_ext7);
                break;
            case 2:
                print_scfg_ext9(scfg_ext9);
                break;
            default:
                break;
        }

        while (1)
        {
            swiWaitForVBlank();

            scanKeys();

            uint16_t keys = keysDown();
            if (keys & KEY_START)
                goto end_loop;
            if (keys & KEY_A)
                break;
        }

        page++;
        if (page == 3)
            page = 0;
    }

end_loop:

    return 0;
}
