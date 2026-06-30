// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2026

// This example shows how to implement a console that supports 256-color
// palettes, 24-bit direct color mode, and setting the color of the foreground
// and the background. It uses a 16-bit bitmap background instead of a tiled
// background.

#include <stdio.h>

#include <nds.h>

#include <regular_size/custom_font_8x8.h>

#include <unusual_sizes/custom_font_4x6.h>
#include <unusual_sizes/custom_font_4x8.h>
#include <unusual_sizes/custom_font_6x8.h>
#include <unusual_sizes/custom_font_6x10.h>

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

// Real size of the console (it's just the size of the screen)
#define CONSOLE_WIDTH       256
#define CONSOLE_HEIGHT      192

// Size of the background layer used for the terminal
#define BACKGROUND_WIDTH    256
#define BACKGROUND_HEIGHT   256

typedef struct
{
    uint16_t palette[256];

    // Size of an individual character
    int character_width;
    int character_height;

    // How characters are arranged in the font bitmap image
    int charset_columns;
    int charset_rows;

    // Number of columns and rows available
    int columns;
    int rows;

    int bgscroll;

    const void *font_bitmap;
}
console_user_info_t;

static console_user_info_t console_user_info;

static void setup_console_palette(PrintConsole *console)
{
    console_user_info_t *info = console->userData;

    // Colors 0-15 correspond to the ANSI and aixterm naming
    info->palette[0] = RGB15(0, 0, 0);     // 30 normal black
    info->palette[1] = RGB15(15, 0, 0);    // 31 normal red
    info->palette[2] = RGB15(0, 15, 0);    // 32 normal green
    info->palette[3] = RGB15(15, 15, 0);   // 33 normal yellow
    info->palette[4] = RGB15(0, 0, 15);    // 34 normal blue
    info->palette[5] = RGB15(15, 0, 15);   // 35 normal magenta
    info->palette[6] = RGB15(0, 15, 15);   // 36 normal cyan
    info->palette[7] = RGB15(24, 24, 24);  // 37 normal white

    info->palette[8] = RGB15(15, 15, 15);  // 40 bright black
    info->palette[9] = RGB15(31, 0, 0);    // 41 bright red
    info->palette[10] = RGB15(0, 31, 0);   // 42 bright green
    info->palette[11] = RGB15(31, 31, 0);  // 43 bright yellow
    info->palette[12] = RGB15(0, 0, 31);   // 44 bright blue
    info->palette[13] = RGB15(31, 0, 31);  // 45 bright magenta
    info->palette[14] = RGB15(0, 31, 31);  // 46 bright cyan
    info->palette[15] = RGB15(31, 31, 31); // 47 bright white

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

                info->palette[code] = RGB15(r >> 3, g >> 3, b >> 3);
            }
        }
    }

    // Colors 232-255 are a grayscale ramp, intentionally leaving out black and white
    for (int gray = 0; gray < 24; gray++)
    {
        int level = gray * 10 + 8;
        int code = 232 + gray;
        info->palette[code] = RGB15(level >> 3, level >> 3, level >> 3);
    }
}

static void custom_console_print_char(PrintConsole *console, char c, int x, int y)
{
    console_user_info_t *info = console->userData;

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
        foreground = info->palette[console->fontCurPal] | BIT(15);
    }

    if (console->sgrBackgroundIsRgb)
    {
        background = RGB15(console->backgroundCurRgb[0] >> 3,
                           console->backgroundCurRgb[1] >> 3,
                           console->backgroundCurRgb[2] >> 3) | BIT(15);
    }
    else
    {
        background = info->palette[console->backgroundCurPal] | BIT(15);
    }

    vu16 *dst = bgGetGfxPtr(console->bgId);

    int char_x = (c % info->charset_columns) * info->character_width;
    int char_y = (c / info->charset_columns) * info->character_height;

    const u16 *src = info->font_bitmap;

    for (int j = 0; j < info->character_height; j++)
    {
        for (int i = 0; i < info->character_width; i++)
        {
            int dst_x = i + (x * info->character_width);
            int dst_y = (info->bgscroll + j + (y * info->character_height)) % BACKGROUND_HEIGHT;
            vu16 *dst_ = dst + (dst_y * CONSOLE_WIDTH) + dst_x;

            int src_x = char_x + i;
            int src_y = char_y + j;
            const u16 *src_ = src + (src_y * (info->charset_columns * info->character_width)) + src_x;

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
    console_user_info_t *info = console->userData;

    uint16_t background = info->palette[CONSOLE_BLACK] | BIT(15);

    vu16 *dst = bgGetGfxPtr(console->bgId);

    for (int j = 0; j < info->character_height; j++)
    {
        for (int i = 0; i < info->character_width; i++)
        {
            int dst_x = i + (x * info->character_width);
            int dst_y = (info->bgscroll + j + (y * info->character_height)) % BACKGROUND_HEIGHT;
            vu16 *dst_ = dst + (dst_y * CONSOLE_WIDTH) + dst_x;
            *dst_ = background;
        }
    }
}

static void custom_console_new_row(PrintConsole *console)
{
    console_user_info_t *info = console->userData;

    console->cursorY++;

    if (console->cursorY >= console->windowHeight)
    {
        console->cursorY = console->windowHeight - 1;

        info->bgscroll = (info->bgscroll + info->character_height) % BACKGROUND_HEIGHT;

        bgSetScroll(console->bgId, 0, info->bgscroll);
        bgUpdate();

        for (int i = 0; i < console->windowWidth; i++)
        {
            custom_console_print_empty(console,
                                       console->windowX + i,
                                       console->windowY + console->windowHeight - 1);
            // Clear two rows just in case. This is needed for sizes that don't
            // divide the screen in an exact number of rows.
            custom_console_print_empty(console,
                                       console->windowX + i,
                                       console->windowY + console->windowHeight);
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

static void setup_console_top(const void *font_bitmap, int character_width,
                              int character_height)
{
    videoSetMode(MODE_5_2D);
    vramSetBankA(VRAM_A_MAIN_BG);

    // Clear background layer
    vu16 *bmp = BG_BMP_RAM_MAIN(0);
    for (int j = 0; j < BACKGROUND_HEIGHT; j++)
    {
        for (int i = 0; i < BACKGROUND_WIDTH; i++)
            *bmp++ = RGB5(0, 0, 0) | BIT(15);
    }

    console_user_info.character_width  = character_width;
    console_user_info.character_height = character_height;

    console_user_info.charset_columns = 32;
    console_user_info.charset_rows    = 8;

    console_user_info.columns = CONSOLE_WIDTH / console_user_info.character_width;
    console_user_info.rows    = CONSOLE_HEIGHT / console_user_info.character_height;

    console_user_info.bgscroll = 0;

    console_user_info.font_bitmap = font_bitmap;

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

    topScreen.userData = &console_user_info;
    topScreen.consoleWidth = console_user_info.columns;
    topScreen.consoleHeight = console_user_info.rows;
    topScreen.windowWidth = console_user_info.columns;
    topScreen.windowHeight = console_user_info.rows;

    setup_console_palette(&topScreen);

    // Allow color commands
    consoleEnhancedColorHandler(&topScreen);

    // Load our custom 16bpp font
    consoleSetFont(&topScreen, &customFont);

    topScreen.PrintChar = custom_console_print;

    // Enable wrap so that we can scroll the terminal efficiently
    bgWrapOn(topScreen.bgId);

    // Reset scroll
    bgSetScroll(topScreen.bgId, 0, 0);
    bgUpdate();
}

static void setup_console_top_font_index(int index)
{
    struct {
        const void *bitmap;
        int char_width;
        int char_height;
    } font[] = {
        { &custom_font_4x6Bitmap[0], 4, 6 },
        { &custom_font_4x8Bitmap[0], 4, 8 },
        { &custom_font_6x8Bitmap[0], 6, 8 },
        { &custom_font_6x10Bitmap[0], 6, 10 },
        { &custom_font_8x8Bitmap[0], 8, 8 },
    };

    setup_console_top(font[index].bitmap,
                      font[index].char_width, font[index].char_height);

    consoleSelect(&bottomScreen);
    consoleSetCursor(&bottomScreen, 0, 4);
    printf("Current size: %dx%d  \n", font[index].char_width, font[index].char_height);
    consoleSelect(&topScreen);
}

static void setup_console_bottom(void)
{
    videoSetModeSub(MODE_0_2D);
    vramSetBankC(VRAM_C_SUB_BG);
    consoleInit(&bottomScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);
}

static int selected_font = 0;

static bool wait_for_input(void)
{
    while (1)
    {
        scanKeys();

        uint16_t keys = keysDown();

        if (keys & KEY_A)
            return true;

        bool refresh_font = false;

        if (keys & KEY_LEFT)
        {
            if (selected_font > 0)
            {
                selected_font--;
                refresh_font = true;
            }
        }
        else if (keys & KEY_RIGHT)
        {
            if (selected_font < 4)
            {
                selected_font++;
                refresh_font = true;
            }
        }

        if (refresh_font)
        {
            setup_console_top_font_index(selected_font);
            return false;
        }

        if (keys & KEY_START)
            exit(0);

        swiWaitForVBlank();
    }
}

static void print_scene(int index)
{
    // Reset all color settings
    printf("\x1b[0m");
    // Clear screen
    printf("\x1b[2J");

    if (index == 0)
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
            if (console_user_info.character_width < 8)
                printf("\n");
        }
    }
    else if (index == 1)
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
    else if (index == 2)
    {
        printf("256-color palette (foreground):\n");
        printf("\n");

        for (int i = 0; i < 256; i++)
            printf("\x1b[38;5;%dm%02X", i, i);
    }
    else if (index == 3)
    {
        printf("256-color palette (background):\n");
        printf("\n");

        for (int i = 0; i < 256; i++)
            printf("\x1b[48;5;%dm%02X", i, i);
    }
    else if (index == 4)
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
    else if (index == 5)
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
    else if (index == 6)
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
}

int main(int argc, char **argv)
{
    // Print instructions to the bottom screen

    setup_console_bottom();

    consoleSelect(&bottomScreen);
    printf("A:          Next screen\n");
    printf("Left/Right: Change font\n");
    consoleSetCursor(&bottomScreen, 0, 23);
    printf("START:      Exit to loader");

    // Print several patterns to the top screen

    setup_console_top_font_index(0);

    consoleSelect(&topScreen);

    int index = 0;

    while (1)
    {
        print_scene(index);

        if (wait_for_input())
        {
            index++;
            if (index == 7)
                index = 0;
        }
    }

    return 0;
}
