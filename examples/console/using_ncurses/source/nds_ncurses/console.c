// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2026

#include <nds.h>

#include <regular_size/custom_font_8x8.h>

#include <unusual_sizes/custom_font_4x6.h>
#include <unusual_sizes/custom_font_4x8.h>
#include <unusual_sizes/custom_font_6x8.h>
#include <unusual_sizes/custom_font_6x10.h>

#include "input.h"

PrintConsole main_console;
PrintConsole debug_console;

// We need a custom font to specify the ASCII offset and the number of
// characters. The other fields aren't used because we're rendering everything
// in custom functions.
static ConsoleFont console_font =
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

static void console_setup_palette(PrintConsole *console)
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

static void console_print_char(PrintConsole *console, char c, int x, int y)
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
        int index = console->fontCurPal;
        if ((console->sgrIntensity) && (index < 8))
            index += 8;

        foreground = info->palette[index] | BIT(15);
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

static void console_print_empty(PrintConsole *console, int x, int y)
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

static void console_new_row(PrintConsole *console)
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
            console_print_empty(console,
                                console->windowX + i,
                                console->windowY + console->windowHeight - 1);
            // Clear two rows just in case. This is needed for sizes that don't
            // divide the screen in an exact number of rows.
            console_print_empty(console,
                                console->windowX + i,
                                console->windowY + console->windowHeight);
        }
    }
}

static bool console_putc(void *con, char c)
{
    if (c == '\0')
        return true;

    PrintConsole *console = con;

    if (console->cursorX >= console->windowWidth)
    {
        console->cursorX = 0;

        console_new_row(console);
    }

    switch (c)
    {
        case '\a': // Bell
        {
            int ch_psg = soundPlayPSG(
                    DutyCycle_50,   // Duty cycle
                    1760,           // Hz
                    127,            // Volume: Max
                    64);            // Panning: Center

            // This isn't ideal, it just blocks the program for a few frames,
            // but it's good enough to be used as an example.
            for (int i = 0; i < 3; i++)
                swiWaitForVBlank();

            soundKill(ch_psg);

            break;
        }
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

            console_print_char(console, ' ',
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
            console_new_row(console);
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

            console_print_char(console, c,
                               console->cursorX + console->windowX,
                               console->cursorY + console->windowY);

            console->cursorX++;
            break;
        }
    }

    return true;
}

static ssize_t console_handle_escape_sequence(void *con, const char *ptr, size_t len)
{
#if 0
    // Normal tracking mode
    // --------------------

    const char *normal_tracking_mode = "[?1000h";

    if (len >= strlen(normal_tracking_mode))
    {
        if (strncmp(normal_tracking_mode, ptr, strlen(normal_tracking_mode)) == 0)
        {
            // This enables press/release and wheel events.
            return strlen(normal_tracking_mode);
        }
    }
#endif

    // Button-event tracking mode
    // --------------------------

    // https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Button-event-tracking

    const char *button_event_tracking_enable = "[?1002h";

    if (len >= strlen(button_event_tracking_enable))
    {
        if (strncmp(button_event_tracking_enable, ptr, strlen(button_event_tracking_enable)) == 0)
        {
            // This enables press/release, motion and wheel events.
            mouse_enable(true);
            return strlen(button_event_tracking_enable);
        }
    }

    const char *button_event_tracking_disable = "[?1002l";

    if (len >= strlen(button_event_tracking_disable))
    {
        if (strncmp(button_event_tracking_disable, ptr, strlen(button_event_tracking_disable)) == 0)
        {
            // This enables press/release, motion and wheel events.
            mouse_enable(true);
            return strlen(button_event_tracking_disable);
        }
    }


    // Report cursor position
    // ----------------------

    const char *report_cursor_position = "[6n";

    if (len >= strlen(report_cursor_position))
    {
        if (strncmp(report_cursor_position, ptr, strlen(report_cursor_position)) == 0)
        {
            PrintConsole *console = con;

            // We need to send ESC[n;mR, where n is the row and m is the column

            char str[20];
            snprintf(str, sizeof(str), "\x1b[%d;%dR",
                     console->cursorY, console->cursorX);

            char *p = &str[0];
            while (*p != '\0')
            {
                keyboardFifoPutc(*p);
                p++;
            }

            return strlen(report_cursor_position);
        }
    }

    // Check for other sequences
    // -------------------------

    // Nothing else to do for now

    return 1;
}

void console_get_character_dimension(int *width, int *height)
{
    *width = console_user_info.character_width;
    *height = console_user_info.character_height;
}

static void console_setup_font(const void *font_bitmap, int character_width,
                               int character_height, int *columns, int *lines)
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

    *columns = console_user_info.columns;
    *lines = console_user_info.rows;

    console_user_info.bgscroll = 0;

    console_user_info.font_bitmap = font_bitmap;

    consoleInitEx(&main_console,
                  2,                  // Background layer
                  BgType_Bmp16,       // 16 BPP bitmap mode
                  BgSize_B16_256x256, // Size of the background layer
                  0,                  // Use map base 0
                  0,                  // Use tile base 0 (unused)
                  0,                  // Use palette index 0 (unused)
                  0,                  // Start from character 0 of the tile base
                  true,               // Main screen
                  false);             // Don't load graphics

    main_console.userData = &console_user_info;
    main_console.consoleWidth = console_user_info.columns;
    main_console.consoleHeight = console_user_info.rows;
    main_console.windowWidth = console_user_info.columns;
    main_console.windowHeight = console_user_info.rows;
    main_console.tabSize = 8;

    console_setup_palette(&main_console);

    // Allow color commands
    consoleEnhancedColorHandler(&main_console);

    // Load our custom 16bpp font
    consoleSetFont(&main_console, &console_font);

    main_console.PrintChar = console_putc;
    main_console.HandleEscapeSequence = console_handle_escape_sequence;

    // Enable wrap so that we can scroll the terminal efficiently
    bgWrapOn(main_console.bgId);

    // Reset scroll
    bgSetScroll(main_console.bgId, 0, 0);
    bgUpdate();
}

void console_setup(int font_index, int *columns, int *lines)
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

    console_setup_font(font[font_index].bitmap,
                       font[font_index].char_width, font[font_index].char_height,
                       columns, lines);

    // Reset attributes and clear screen
    printf("\x1b[0m");
    printf("\x1b[2J");

    // Print some debug information
    consoleSelect(&debug_console);
    consoleSetCursor(&debug_console, 0, 4);
    printf("Font size: %dx%d  \n",
           font[font_index].char_width, font[font_index].char_height);
    consoleSelect(&main_console);

    // I'm not sure if this is needed, but it doesn't hurt
    char str[10];
    snprintf(str, sizeof(str), "%d", *columns);
    setenv("COLUMNS", str, true);
    snprintf(str, sizeof(str), "%d", *lines);
    setenv("LINES", str, true);
}

// -----------------------------------------------------------------------------

void debug_console_setup(void)
{
    videoSetModeSub(MODE_0_2D);
    vramSetBankC(VRAM_C_SUB_BG);
    consoleInit(&debug_console, 2, BgType_Text4bpp, BgSize_T_256x256, 24, 2, false, true);
}
