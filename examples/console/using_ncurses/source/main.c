// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2026

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ncurses.h>

#ifdef __BLOCKSDS__
#include "nds_ncurses/setup.h"
#endif

int show_menu(void)
{
    clear();

    attron(A_BOLD);
    mvwprintw(stdscr, 0, 6, "BlocksDS ncurses demo");
    mvwprintw(stdscr, 1, 6, "=====================");
    attroff(A_BOLD);

    wmove(stdscr, 3, 0);
    wprintw(stdscr, "1: Text attributes scene\n");
    wprintw(stdscr, "2: 256 color text\n");
    wprintw(stdscr, "3: Keyboard input\n");
    wprintw(stdscr, "4: Mouse input\n");
    wprintw(stdscr, "\n");
    wprintw(stdscr, "Q: Exit demo!\n");

    refresh();

    while (1)
    {
        wmove(stdscr, 10, 0);
        wprintw(stdscr, "Console size: %dx%d", LINES, COLS);

        refresh();

        int ch = getch(); // Wait for user input
        if (ch > 0)
        {
            if ((ch >= '1') && (ch <= '4'))
                return ch;

            if ((ch == 'q') || (ch == 'Q'))
                return -1;
        }
    }

    clear();
}

void show_scene(int index)
{
    clear();

    if (index == '1')
    {
        attron(A_BOLD);
        mvwprintw(stdscr, 0, 6, "Text attributes");
        mvwprintw(stdscr, 1, 6, "===============");
        attroff(A_BOLD);

        wmove(stdscr, 3, 0);
        wprintw(stdscr, "normal text\n");
        attron(A_BOLD);
        wprintw(stdscr, "bold text\n");
        attroff(A_BOLD);

        init_pair(1, COLOR_BLACK, COLOR_WHITE);
        init_pair(2, COLOR_RED, COLOR_BLACK);
        init_pair(3, COLOR_GREEN, COLOR_BLACK);
        init_pair(4, COLOR_YELLOW, COLOR_BLACK);
        init_pair(5, COLOR_BLUE, COLOR_BLACK);
        init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(7, COLOR_CYAN, COLOR_BLACK);
        init_pair(8, COLOR_WHITE, COLOR_BLACK);

        const char *colornames[] = {
            "black", "red", "green", "yellow", "blue", "magenta", "cyan", "white"
        };

        for (int i = 0; i < 8; i++)
        {
            int pair = i + 1;

            wmove(stdscr, 6 + i, 0);

            attron(COLOR_PAIR(pair));
            wprintw(stdscr, "%s", colornames[i]);
            attroff(COLOR_PAIR(pair));
        }

        for (int i = 0; i < 8; i++)
        {
            int pair = i + 1;

            wmove(stdscr, 6 + i, 12);

            attron(A_BOLD | COLOR_PAIR(pair));
            wprintw(stdscr, "bright %s", colornames[i]);
            attroff(A_BOLD | COLOR_PAIR(pair));
        }

        wmove(stdscr, 18, 0);
        wprintw(stdscr, "Press any key to return");

        refresh();

        while (1)
        {
            wmove(stdscr, 16, 0);
            wprintw(stdscr, "Console size: %dx%d", COLS, LINES);

            refresh();

            int ch = getch(); // Wait for user input
            if (ch > 0)
                break;
        }
    }
    else if (index == '2')
    {
        attron(A_BOLD);
        mvwprintw(stdscr, 0, 6, "256 color text");
        mvwprintw(stdscr, 1, 6, "==============");
        attroff(A_BOLD);

        for (int i = 0; i < 256; i++)
            init_pair(i + 1, i, COLOR_BLACK);

        for (int i = 0; i < 256; i++)
        {
            int pair = i + 1;

            wmove(stdscr, 3 + (i / 32), (i % 32));
            attron(COLOR_PAIR(pair));
            wprintw(stdscr, "%X", i & 0xF);
            attroff(COLOR_PAIR(pair));
        }

        wmove(stdscr, 16, 0);
        wprintw(stdscr, "Press any key to return");

        refresh();

        while (1)
        {
            wmove(stdscr, 14, 0);
            wprintw(stdscr, "Console size: %dx%d", COLS, LINES);

            refresh();

            int ch = getch(); // Wait for user input
            if (ch > 0)
                break;
        }
    }
    else if (index == '3')
    {
        attron(A_BOLD);
        mvwprintw(stdscr, 0, 6, "Keyboard input");
        mvwprintw(stdscr, 1, 6, "==============");
        attroff(A_BOLD);

        wmove(stdscr, 16, 0);
        wprintw(stdscr, "Press Q to return");

        refresh();

        while (1)
        {
            wmove(stdscr, 18, 0);
            wprintw(stdscr, "Console size: %dx%d", COLS, LINES);

            refresh();

            wmove(stdscr, 10, 10);

            int ch = getch(); // Wait for user input
            if (ch > 0)
            {
                if ((ch == 'Q') || (ch == 'q'))
                    break;

                if (ch == KEY_UP)
                    wprintw(stdscr, "[Up]       ");
                else if (ch == KEY_DOWN)
                    wprintw(stdscr, "[Down]     ");
                else if (ch == KEY_LEFT)
                    wprintw(stdscr, "[Left]     ");
                else if (ch == KEY_RIGHT)
                    wprintw(stdscr, "[Right]    ");
                else if (ch == KEY_ENTER)
                    wprintw(stdscr, "[Enter]    ");
                else if (ch == KEY_BACKSPACE)
                    wprintw(stdscr, "[Backspace]");
                else if ((ch < 32) || (ch > 255))
                    wprintw(stdscr, "<%d>       ", ch);
                else
                    wprintw(stdscr, "[%c]       ", ch);
            }
        }
    }
    else if (index == '4')
    {
        attron(A_BOLD);
        mvwprintw(stdscr, 0, 6, "Mouse input");
        mvwprintw(stdscr, 1, 6, "===========");
        attroff(A_BOLD);

        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_YELLOW, COLOR_BLACK);
        init_pair(4, COLOR_BLUE, COLOR_BLACK);
        init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(6, COLOR_CYAN, COLOR_BLACK);
        init_pair(7, COLOR_WHITE, COLOR_BLACK);

        wmove(stdscr, 16, 0);
        wprintw(stdscr, "Press Q to return");

        refresh();

        // Fetch any pending mouse event from the queue
        while (1)
        {
            MEVENT event;
            if (getmouse(&event) != OK)
                break;
        }

        int pair = 1;

        while (1)
        {
            wmove(stdscr, 18, 0);
            wprintw(stdscr, "Console size: %dx%d", COLS, LINES);

            refresh();

            int ch = getch(); // Wait for user input
            if (ch > 0)
            {
                if ((ch == 'Q') || (ch == 'q'))
                    break;

                if (ch == KEY_MOUSE)
                {
                    MEVENT event;

                    if (getmouse(&event) == OK)
                    {
                        if (event.bstate & REPORT_MOUSE_POSITION)
                        {
                            wmove(stdscr, event.y, event.x);

                            attron(A_BOLD | COLOR_PAIR(pair));
                            wprintw(stdscr, "@");
                            attroff(A_BOLD | COLOR_PAIR(pair));
                        }
                        else if (event.bstate & BUTTON1_PRESSED)
                        {
                            wmove(stdscr, event.y, event.x);

                            attron(A_BOLD | COLOR_PAIR(pair));
                            wprintw(stdscr, "O");
                            attroff(A_BOLD | COLOR_PAIR(pair));
                        }
                        else if (event.bstate & BUTTON1_RELEASED)
                        {
                            wmove(stdscr, event.y, event.x);

                            attron(A_BOLD | COLOR_PAIR(pair));
                            wprintw(stdscr, "X");
                            attroff(A_BOLD | COLOR_PAIR(pair));

                            pair++;
                            if (pair == 8)
                                pair = 1;
                        }
                    }
                    else
                    {
                        wprintw(stdscr, "\n[Mouse event fail]");
                    }
                }

            }
        }
    }
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

#ifdef __BLOCKSDS__
    nds_setup_input_output();
#endif

#ifdef DEBUG
    curses_trace(TRACE_MAXIMUM);
#endif

    // Initialize ncurses
    initscr();

    // Disable line buffering
    raw();
    // Get processed keys instead of escape sequences
    keypad(stdscr, TRUE);
    // Don't print the characters on the screen as keys are pressed
    noecho();
    // Disable line buffering and erase/kill character-processing
    cbreak();
    // Set non-blocking input mode. -1 = blocking input mode, 0 = non-blocking, no delay
    timeout(0);
    // Hide cursor
    curs_set(0);

    if (has_colors() == FALSE)
    {
        endwin();
        printf("Your terminal does not support color\n");
        while (1)
            sleep(1);
    }

    start_color();

    // Ask ncurses to look for press/release events and for mouse movements
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
    // Disable delay when handling mouse escape sequences
    mouseinterval(0);

    if (has_mouse() == FALSE)
    {
        endwin();
        printf("Your terminal does not support mouse\n");
        while (1)
            sleep(1);
    }

    while (1)
    {
        int scene = show_menu();
        if (scene == -1)
            break;

        show_scene(scene);
    }

    endwin();

    return 0;
}
