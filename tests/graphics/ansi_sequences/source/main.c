// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

// Information about ANSI escape codes:
//
//     https://en.wikipedia.org/wiki/ANSI_escape_code#CSI_(Control_Sequence_Introducer)_sequences
//
// Check the documentation of libnds to see which ones are actually supported.

#include <stdio.h>

#include <nds.h>

static PrintConsole topScreen;
static PrintConsole bottomScreen;

void wait_for_press(const char *reason)
{
    printf("Press A to %s\n", reason);
    while (1)
    {
        swiWaitForVBlank();
        scanKeys();
        if (keysDown() & KEY_A)
            break;
    }
}

void clear_screens(void)
{
    consoleSelect(&topScreen);
    consoleClear();
    consoleSelect(&bottomScreen);
    consoleClear();
}

void clear_console(void)
{
    const char *codes[4] = { "[J", "[0J", "[1J", "[2J" };

    const char *meaning[4] =
    {
        "Clear screen after cursor",
        "Clear screen after cursor",
        "Clear screen before cursor",
        "Clear all, set cursor to (0, 0)",
    };

    for (int i = 0; i < 4; i++)
    {
        clear_screens();

        consoleSelect(&bottomScreen);
        printf("ANSI sequence: \"\\x1b%s\"\n", codes[i]);
        printf("\n");
        printf("Meaning:\n");
        printf("%s\n", meaning[i]);
        printf("\n");

        // Fill the top screen with text
        consoleSelect(&topScreen);
        for (int j = 0; j < 24; j++)
            printf("................................");

        // Set the cursor in the middle
        consoleSetCursor(NULL, 15, 12);
        printf("0");

        consoleSelect(&bottomScreen);
        wait_for_press("reveal");

        // Run code and print the cursor again to see where it is now
        consoleSelect(&topScreen);
        printf("\x1b%s", codes[i]);
        printf("1");

        consoleSelect(&bottomScreen);
        wait_for_press("continue");
    }

    clear_screens();
}

void clear_line(void)
{
    const char *codes[4] = { "[K", "[0K", "[1K", "[2K" };

    const char *meaning[4] = {
        "Clear line after cursor",
        "Clear line after cursor",
        "Clear line before cursor",
        "Clear line, preserve cursor",
    };

    for (int i = 0; i < 4; i++)
    {
        clear_screens();

        consoleSelect(&bottomScreen);
        printf("ANSI sequence: \"\\x1b%s\"\n", codes[i]);
        printf("\n");
        printf("Meaning:\n");
        printf("%s\n", meaning[i]);
        printf("\n");

        // Fill the top screen with text
        consoleSelect(&topScreen);
        for (int j = 0; j < 24; j++)
            printf("................................");

        // Set the cursor in the middle
        consoleSetCursor(NULL, 15, 12);
        printf("0");

        consoleSelect(&bottomScreen);
        wait_for_press("reveal");

        // Run code and print the cursor again to see where it is now
        consoleSelect(&topScreen);
        printf("\x1b%s", codes[i]);
        printf("1");

        consoleSelect(&bottomScreen);
        wait_for_press("continue");
    }

    clear_screens();
}

void move_cursor(void)
{
    clear_screens();

    const char *codes[8] = {
        "[15C",
        "[8B",
        "[40B",
        "[7D",
        "[40D",
        "[9A",
        "[40A",
        "[40C",
    };

    const char *meaning[8] = {
        "Move right: 15 spaces",
        "Move down:  8 spaces",
        "Move down:  40 spaces",
        "Move left:  7 spaces",
        "Move left:  40 spaces",
        "Move up:    9 spaces",
        "Move up:    40 spaces",
        "Move right: 40 spaces",
    };

    consoleSelect(&topScreen);
    printf("0");

    for (int i = 0; i < 8; i++)
    {
        consoleSelect(&bottomScreen);
        consoleClear();
        printf("ANSI sequence: \"\\x1b%s\"\n", codes[i]);
        printf("\n");
        printf("Meaning:\n");
        printf("%s\n", meaning[i]);
        printf("\n");

        consoleSelect(&topScreen);

        consoleSelect(&bottomScreen);
        wait_for_press("reveal");

        // Run code and print the cursor again to see where it is now
        consoleSelect(&topScreen);
        printf("\x1b%s", codes[i]);
        printf("%d", i + 1);

        consoleSelect(&bottomScreen);
        wait_for_press("continue");
    }

    clear_screens();
}

void set_cursor(void)
{
    clear_screens();

    const char *codes[6] = {
        "[10;1H",
        "[2;20f",
        "[s",
        "[2;50f",
        "[u",
        "[70;3H",

    };

    const char *meaning[6] = {
        "Move to (1, 10)",
        "Move to (20, 2)",
        "Save cursor",
        "Move to (50, 2)",
        "Restore cursor",
        "Move to (3, 70)",
    };

    consoleSelect(&topScreen);
    printf("0");

    for (int i = 0; i < 6; i++)
    {
        consoleSelect(&bottomScreen);
        consoleClear();
        printf("ANSI sequence: \"\\x1b%s\"\n", codes[i]);
        printf("\n");
        printf("Meaning:\n");
        printf("%s\n", meaning[i]);
        printf("\n");

        consoleSelect(&topScreen);

        consoleSelect(&bottomScreen);
        wait_for_press("reveal");

        // Run code and print the cursor again to see where it is now
        consoleSelect(&topScreen);
        printf("\x1b%s", codes[i]);
        printf("%d", i + 1);

        consoleSelect(&bottomScreen);
        wait_for_press("continue");
    }

    clear_screens();
}

void test_colors(void)
{
    clear_screens();

    consoleSelect(&bottomScreen);
    printf("Console palettes\n");
    printf("\n");

    consoleSelect(&topScreen);
    // Colors (30 to 37): Black, Red, Green, Yellow, Blue, Magenta, Cyan, White
    // Setting intensity to 1 will make them brighter.
    char c = 'A';
    for (int intensity = 0; intensity < 2; intensity++)
        for (int color = 30; color < 38; color++)
            printf("\x1b[%d;%dm%c", color, intensity, c++);

    // Reset color to white
    printf("\x1b[39;0m");

    printf("\n");
    printf("\n");
    printf("Default color");

    consoleSelect(&bottomScreen);
    wait_for_press("continue");
}

int main(int argc, char **argv)
{
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(&topScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    consoleInit(&bottomScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);

    // Now test all ANSI escape sequences supported by libnds

    clear_console();
    clear_line();
    move_cursor();
    set_cursor();
    test_colors();

    clear_screens();
    printf("\x1b[23;0HPress START to exit to loader");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        if (keysDown() & KEY_START)
            break;
    }

    return 0;
}
