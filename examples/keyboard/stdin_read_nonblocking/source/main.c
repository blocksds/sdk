// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2026

// This example shows how to do non-blocking reads from the keyboard using
// standard system call functions. This can be useful if you're porting an
// application that uses stdin for input.

#include <stdio.h>
#include <sys/ioctl.h>

#include <nds.h>

int main(int argc, char **argv)
{
    consoleDemoInit();

    // Initialize demo keyboard and load graphics to VRAM
    keyboardDemoInit();
    // Display keyboard from the start so that the user can press anytime
    keyboardShow();

    // Enable non-blocking mode
    int opt = 1;
    ioctl(STDIN_FILENO, FIONBIO, &opt);

    // You can also do it like this (if you include <fcntl.h>):
    //int flags = fcntl(fd, F_GETFL, 0);
    //fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    char string[256];
    string[0] = '\0';

    while (1)
    {
        swiWaitForVBlank();

        // Update the key state and let the keyboard handler detect key presses.
        // The key presses will be added to the keyboard FIFO.
        scanKeys();
        keyboardFifoUpdate();

        uint16_t keys = keysDown();

        consoleClear();

        printf("Press START to exit to loader\n");
        printf("\n");
        printf("Press the keyboard to save\n");
        printf("key presses to an internal FIFO.\n");
        printf("Press SELECT to call read().\n");
        printf("\n");
        printf("Read size: %zu chars\n", sizeof(string) - 1);

        // Ask how many characters are there waiting to be read
        int bytes_available = 0;
        if (ioctl(STDIN_FILENO, FIONREAD, &bytes_available) < 0)
            bytes_available = -1;
        printf("Available characters: %d\n", bytes_available);

        printf("\n");

        printf("[%s]\n", string);
        printf("Length: %d\n", strlen(string));

        if (keys & KEY_SELECT)
        {
            // This will read whatever is in the FIFO buffer until there are no
            // more characters to be read or until a '\n' character is found.
            ssize_t chars = read(STDIN_FILENO, string, sizeof(string) - 1);
            if (chars > 0)
            {
                // Make sure that the string has a valid terminator.
                string[chars] = '\0';
            }
        }

        if (keys & KEY_START)
            break;
    }

    return 0;
}
