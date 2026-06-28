// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2026

// This example shows how to implement a console that supports 256-color
// palettes, 24-bit direct color mode, and setting the color of the foreground
// and the background. It uses a 16-bit bitmap background instead of a tiled
// background.

#include <stdio.h>

#include <nds.h>

#include <custom_font_4x6.h>
#include <custom_font_4x8.h>
#include <custom_font_6x8.h>
// TODO: Support switching fonts at runtime

static PrintConsole topScreen;
static PrintConsole bottomScreen;

// We need a custom font to specify the ASCII offset and the number of
// characters. The other fields aren't used because we're rendering everything
// in custom functions.
static ConsoleFont customFont =
{
    .gfx = NULL,
    .pal = NULL,
    .numColors = 0,
    .bpp = 16,
    .asciiOffset = 0,
    .numChars = 256,
};

static int console_bgscroll = 0;
static uint16_t console_palette[256];

// Size of an individual character
#define CHARACTER_WIDTH     4
#define CHARACTER_HEIGHT    6

// How characters are arranged in the font bitmap image
#define CHARSET_COLUMNS     32
#define CHARSET_ROWS        8

// Real size of the console (it's just the size of the screen)
#define CONSOLE_WIDTH       256
#define CONSOLE_HEIGHT      192

// Number of columns and rows available
#define CONSOLE_COLUMNS     (CONSOLE_WIDTH / CHARACTER_WIDTH)
#define CONSOLE_ROWS        (CONSOLE_HEIGHT / CHARACTER_HEIGHT)

// Size of the background layer used for the terminal
#define BACKGROUND_WIDTH    256
#define BACKGROUND_HEIGHT   256

void setup_console_palette(void)
{
    // Colors 0-15 correspond to the ANSI and aixterm naming
    console_palette[0] = RGB15(0, 0, 0);     // 30 normal black
    console_palette[1] = RGB15(15, 0, 0);    // 31 normal red
    console_palette[2] = RGB15(0, 15, 0);    // 32 normal green
    console_palette[3] = RGB15(15, 15, 0);   // 33 normal yellow
    console_palette[4] = RGB15(0, 0, 15);    // 34 normal blue
    console_palette[5] = RGB15(15, 0, 15);   // 35 normal magenta
    console_palette[6] = RGB15(0, 15, 15);   // 36 normal cyan
    console_palette[7] = RGB15(24, 24, 24);  // 37 normal white

    console_palette[8] = RGB15(15, 15, 15);  // 40 bright black
    console_palette[9] = RGB15(31, 0, 0);    // 41 bright red
    console_palette[10] = RGB15(0, 31, 0);   // 42 bright green
    console_palette[11] = RGB15(31, 31, 0);  // 43 bright yellow
    console_palette[12] = RGB15(0, 0, 31);   // 44 bright blue
    console_palette[13] = RGB15(31, 0, 31);  // 45 bright magenta
    console_palette[14] = RGB15(0, 31, 31);  // 46 bright cyan
    console_palette[15] = RGB15(31, 31, 31); // 47 bright white

    // Colors 16-231 are a 6x6x6 color cube
    for (int red = 0; red < 6; red++)
    {
        for (int green = 0; green < 6; green++)
        {
            for (int blue = 0; blue < 6; blue++)
            {
                int r, g, b;

                int code = 16 + (red * 36) + (green * 6) + blue;

                if (red)
                    r = red * 40 + 55;
                else
                    r = 0;

                if (green)
                    g = green * 40 + 55;
                else
                    g = 0;

                if (blue)
                    b = blue * 40 + 55;
                else
                    b = 0;

                console_palette[code] = RGB15(r >> 3, g >> 3, b >> 3);
            }
        }
    }

    // Colors 232-255 are a grayscale ramp, intentionally leaving out black and white
    for (int gray = 0; gray < 24; gray++)
    {
        int level = gray * 10 + 8;
        int code = 232 + gray;
        console_palette[code] = RGB15(level >> 3, level >> 3, level >> 3);
    }
}

static void custom_console_print_char(PrintConsole *console, char c, int x, int y)
{
    uint16_t foreground = 0;
    uint16_t background = 0;

    if (console->sgrForegroundIsRgb)
    {
        foreground = RGB15(console->fontCurRgb[0] >> 3,
                           console->fontCurRgb[1] >> 3,
                           console->fontCurRgb[2] >> 3) | BIT(15);
    }
    else
    {
        foreground = console_palette[console->fontCurPal] | BIT(15);
    }

    if (console->sgrBackgroundIsRgb)
    {
        background = RGB15(console->backgroundCurRgb[0] >> 3,
                           console->backgroundCurRgb[1] >> 3,
                           console->backgroundCurRgb[2] >> 3) | BIT(15);
    }
    else
    {
        background = console_palette[console->backgroundCurPal] | BIT(15);
    }

    vu16 *dst = bgGetGfxPtr(console->bgId);

    int char_x = (c % CHARSET_COLUMNS) * CHARACTER_WIDTH;
    int char_y = (c / CHARSET_COLUMNS) * CHARACTER_HEIGHT;

    u16 *src = (u16 *)&custom_font_4x6Bitmap[0];

    for (int j = 0; j < CHARACTER_HEIGHT; j++)
    {
        for (int i = 0; i < CHARACTER_WIDTH; i++)
        {
            int dst_x = i + (x * CHARACTER_WIDTH);
            int dst_y = (console_bgscroll + j + (y * CHARACTER_HEIGHT)) % BACKGROUND_HEIGHT;
            vu16 *dst_ = dst + (dst_y * CONSOLE_WIDTH) + dst_x;

            int src_x = char_x + i;
            int src_y = char_y + j;
            u16 *src_ = src + (src_y * (CHARSET_COLUMNS * CHARACTER_WIDTH)) + src_x;

            uint16_t color;

            if (*src_)
                color = foreground;
            else
                color = background;

            *dst_ = color;
        }
    }
}

static void custom_console_print_empty(PrintConsole *console, int x, int y)
{
    uint16_t background = console_palette[CONSOLE_BLACK] | BIT(15);

    vu16 *dst = bgGetGfxPtr(console->bgId);

    for (int j = 0; j < CHARACTER_HEIGHT; j++)
    {
        for (int i = 0; i < CHARACTER_WIDTH; i++)
        {
            int dst_x = i + (x * CHARACTER_WIDTH);
            int dst_y = (console_bgscroll + j + (y * CHARACTER_HEIGHT)) % BACKGROUND_HEIGHT;
            vu16 *dst_ = dst + (dst_y * CONSOLE_WIDTH) + dst_x;
            *dst_ = background;
        }
    }
}

static void custom_console_new_row(PrintConsole *console)
{
    console->cursorY++;

    if (console->cursorY >= console->windowHeight)
    {
        console->cursorY = console->windowHeight - 1;

        console_bgscroll = (console_bgscroll + CHARACTER_HEIGHT) % BACKGROUND_HEIGHT;

        bgSetScroll(console->bgId, 0, console_bgscroll);
        bgUpdate();

        for (int i = 0; i < console->windowWidth; i++)
        {
            custom_console_print_empty(console,
                                      console->windowX + i,
                                      console->windowY + console->windowHeight - 1);
        }
    }
}

static bool custom_console_print(void *con, char c)
{
    if (c == '\0')
        return true;

    PrintConsole *console = con;

    if (console->cursorX >= console->windowWidth)
    {
        console->cursorX = 0;

        custom_console_new_row(console);
    }

    switch (c)
    {
        case '\b':
        {
            console->cursorX--;

            if (console->cursorX < 0)
            {
                if (console->cursorY > 0)
                {
                    console->cursorX = console->windowX - 1;
                    console->cursorY--;
                }
                else
                {
                    console->cursorX = 0;
                }
            }

            custom_console_print_char(console, ' ',
                                      console->cursorX + console->windowX,
                                      console->cursorY + console->windowY);
            break;
        }
        case '\t':
        {
            int spaces = console->tabSize
                       - ((console->cursorX) % (console->tabSize));

            console->cursorX += spaces;
            break;
        }
        case '\n':
            custom_console_new_row(console);
            // fallthrough
        case '\r':
            console->cursorX = 0;
            break;

        default:
        {
            if (c < console->font.asciiOffset)
                c = ' ';
            if (c >= console->font.asciiOffset + console->font.numChars)
                c = ' ';

            custom_console_print_char(console, c,
                                      console->cursorX + console->windowX,
                                      console->cursorY + console->windowY);

            console->cursorX++;
            break;
        }
    }

    return true;
}

void wait_for_input(void)
{
    while (1)
    {
        scanKeys();

        uint16_t keys = keysDown();
        if (keys & KEY_A)
            break;
        if (keys & KEY_START)
            exit(0);

        swiWaitForVBlank();
    }
}

void setup_console_top(void)
{
    setup_console_palette();

    videoSetMode(MODE_5_2D);
    vramSetBankA(VRAM_A_MAIN_BG);

    // Clear background layer
    vu16 *bmp = BG_BMP_RAM_MAIN(0);
    for (int j = 0; j < BACKGROUND_HEIGHT; j++)
    {
        for (int i = 0; i < BACKGROUND_WIDTH; i++)
            *bmp++ = RGB5(0, 0, 0) | BIT(15);
    }

    consoleInitEx(&topScreen,
                  2,                  // Background layer
                  BgType_Bmp16,       // 16 BPP bitmap mode
                  BgSize_B16_256x256, // Size of the background layer
                  0,                  // Use map base 0
                  0,                  // Use tile base 0 (unused)
                  0,                  // Use palette index 0 (unused)
                  0,                  // Start from character 0 of the tile base
                  true,               // Main screen
                  false);             // Don't load graphics

    topScreen.consoleWidth = CONSOLE_COLUMNS;
    topScreen.consoleHeight = CONSOLE_ROWS;
    topScreen.windowWidth = CONSOLE_COLUMNS;
    topScreen.windowHeight = CONSOLE_ROWS;

    // Allow color commands
    consoleEnhancedColorHandler(&topScreen);

    // Load our custom 16bpp font
    consoleSetFont(&topScreen, &customFont);

    topScreen.PrintChar = custom_console_print;

    // Enable wrap so that we can scroll the terminal efficiently
    bgWrapOn(topScreen.bgId);
}

void setup_console_bottom(void)
{
    videoSetModeSub(MODE_0_2D);
    vramSetBankC(VRAM_C_SUB_BG);
    consoleInit(&bottomScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);
}

int main(int argc, char **argv)
{
    defaultExceptionHandler();
    consoleDebugInit(DebugDevice_NOCASH);

    setup_console_top();
    setup_console_bottom();

    // Print instructions to the bottom screen

    consoleSelect(&bottomScreen);
    printf("A: Next screen");
    consoleSetCursor(&bottomScreen, 0, 23);
    printf("START: Exit to loader");

    // Print several patterns to the bottom screen

    consoleSelect(&topScreen);

    // Reset all color settings
    printf("\x1b[0m");
    // Clear screen
    printf("\x1b[2J");

    while (1)
    {
        {
            printf("Characters:\n");
            printf("\n");

            printf("\x1b[0m");
            for (int j = 0; j < 8; j++)
            {
                for (int i = 0; i < 32; i++)
                {
                    char c = i + j * 32;

                    // Skip special characters
                    if ((c == '\0') || (c == '\n') || (c == '\r') ||
                        (c == '\t') || (c == '\b'))
                    {
                        c = ' ';
                    }

                    printf("%c", c);
                }

                // Only add a newline if 32 characters are narrower than the screen
                if (CHARACTER_WIDTH < 8)
                    printf("\n");
            }
        }

        printf("\x1b[0m");
        wait_for_input();
        printf("\x1b[2J");

        {
            printf("16-color palette:\n");
            printf("\n");

            printf("\x1b[22m");
            for (int i = 0; i <= 7; i++)
                printf("\x1b[%dm%X", 30 + i, i);
            for (int i = 0; i <= 7; i++)
                printf("\x1b[%dm%X", 90 + i, i);
            printf("\n");

            printf("\x1b[0m");
            for (int i = 0; i <= 7; i++)
                printf("\x1b[%dm%X", 40 + i, i);
            for (int i = 0; i <= 7; i++)
                printf("\x1b[%dm%X", 100 + i, i);
            printf("\n");
        }

        printf("\x1b[0m");
        wait_for_input();
        printf("\x1b[2J");

        {
            printf("256-color palette (foreground):\n");
            printf("\n");

            for (int i = 0; i < 256; i++)
                printf("\x1b[38;5;%dm%02X", i, i);
        }

        printf("\x1b[0m");
        wait_for_input();
        printf("\x1b[2J");

        {
            printf("256-color palette (background):\n");
            printf("\n");

            for (int i = 0; i < 256; i++)
                printf("\x1b[48;5;%dm%02X", i, i);
        }

        printf("\x1b[0m");
        wait_for_input();
        printf("\x1b[2J");

        {
            printf("24 bit mode (foreground):\n");
            printf("\n");

            int count = 0;

            for (int r = 1; r < 5; r++)
            {
                for (int g = 1; g < 5; g++)
                {
                    for (int b = 1; b < 5; b++)
                    {
                        int red = r * 63;
                        int green = g * 63;
                        int blue = b * 63;
                        printf("\x1b[38;2;%d;%d;%dm%02X,%02X,%02X  ",
                            red, green, blue, red, green, blue);

                        count++;
                        if (count == 4)
                        {
                            printf("\n");
                            count = 0;
                        }
                    }
                }
            }
        }

        printf("\x1b[0m");
        wait_for_input();
        printf("\x1b[2J");

        {
            printf("24 bit mode (background):\n");
            printf("\n");

            int count = 0;

            for (int r = 1; r < 5; r++)
            {
                for (int g = 1; g < 5; g++)
                {
                    for (int b = 1; b < 5; b++)
                    {
                        int red = r * 63;
                        int green = g * 63;
                        int blue = b * 63;
                        printf("\x1b[48;2;%d;%d;%dm%02X,%02X,%02X\x1b[0m  ",
                            red, green, blue, red, green, blue);

                        count++;
                        if (count == 4)
                        {
                            printf("\n");
                            count = 0;
                        }
                    }
                }
            }
        }

        printf("\x1b[0m");
        wait_for_input();
        printf("\x1b[2J");

        {
            consoleSetCursor(&topScreen, 0, 21);

            printf("\x1b[0m");
            for (int i = 0; i < 256; i++)
            {
                printf("\x1b[38;5;%dm", i);
                printf("This is a test string %d\n", i);
                swiWaitForVBlank();
            }

            printf("\x1b[0m");
            for (int i = 0; i < 256; i++)
            {
                printf("\x1b[48;5;%dm", i);
                printf("This is a test string %d\n", i);
                swiWaitForVBlank();
            }

            printf("\x1b[0m");
            consoleSetCursor(&topScreen, 5, 9);
            printf("                   ");
            consoleSetCursor(&topScreen, 5, 10);
            printf("    You can also   ");
            consoleSetCursor(&topScreen, 5, 11);
            printf("                   ");
            consoleSetCursor(&topScreen, 5, 12);
            printf("  move the cursor. ");
            consoleSetCursor(&topScreen, 5, 13);
            printf("                   ");
        }

        printf("\x1b[0m");
        wait_for_input();
        printf("\x1b[2J");
    }

    return 0;
}
