// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <filesystem.h>
#include <nds.h>

#include "teak/teak1_tlf.h"

__attribute__((noreturn)) void WaitLoop(void)
{
    printf("Press START to exit");
    while (1)
    {
        swiWaitForVBlank();
        scanKeys();
        if (keysHeld() & KEY_START)
            exit(0);
    }
}

void *load_file(const char *path)
{
    FILE *file = fopen(path, "rb");
    if (file == NULL)
        return NULL;

    if (fseek(file, 0, SEEK_END) != 0)
    {
        fclose(file);
        return NULL;
    }

    size_t size = ftell(file);
    rewind(file);

    void *buffer = malloc(size);
    if (buffer == NULL)
    {
        fclose(file);
        return NULL;
    }

    if (fread(buffer, 1, size, file) != size)
    {
        fclose(file);
        return NULL;
    }

    return buffer;
}

void load_tlf(const void *ptr)
{
    if (dspExecuteDefaultTLF(ptr) != DSP_EXEC_OK)
    {
        printf("Failed to execute TLF");
        WaitLoop();
    }
}

int main(int argc, char **argv)
{
    // Setup sub screen for the text console
    consoleDemoInit();

    if (!isDSiMode())
    {
        printf("This demo must run on a DSi");
        WaitLoop();
    }

    if (!nitroFSInit(NULL))
    {
        printf("nitroFSInit failed.\n");
        WaitLoop();
    }

    void *tlf_from_nitrofs = load_file("teak/teak2.tlf");
    if (tlf_from_nitrofs == NULL)
    {
        printf("Failed to load TLF file from NitroFS");
        WaitLoop();
    }

    uint16_t cmd0 = 0;
    int16_t cmd1 = 15;
    int16_t cmd2 = 5;

    uint16_t rep0 = 0;
    int16_t rep1 = 0;
    int16_t rep2 = 0;

    // Load the binary from ARM9 data initially
    load_tlf(teak1_tlf);
    char *tlf_name = "teak1";

    while (1)
    {
        // Synchronize game loop to the screen refresh
        swiWaitForVBlank();

        // Clear console
        printf("\x1b[2J");

        // Print some controls
        printf("Current TLF: %s\n", tlf_name);
        printf("\n");
        printf("\n");
        printf("L:      Load TLF1\n");
        printf("R:      Load TLF2\n");
        printf("PAD:    Change values\n");
        printf("\n");
        printf("\n");
        printf("START:  Exit to loader\n");
        printf("\n");
        printf("\n");

        // Handle user input
        // -----------------

        scanKeys();

        uint16_t keys = keysHeld();
        uint16_t keys_down = keysDown();

        if (keys & KEY_LEFT)
            cmd1++;
        if (keys & KEY_RIGHT)
            cmd1--;

        if (keys & KEY_UP)
            cmd2++;
        if (keys & KEY_DOWN)
            cmd2--;

        if (keys_down & KEY_L)
        {
            // Load the binary from ARM9 data initially
            load_tlf(teak1_tlf);
            tlf_name = "teak1";
        }
        if (keys_down & KEY_R)
        {
            // Load the binary from NitroFS
            load_tlf(tlf_from_nitrofs);
            tlf_name = "teak2";
        }

        if (keys & KEY_START)
            break;

        // DSP communications

        printf("CMD: %u %d %d\n", cmd0, cmd1, cmd2);

        cmd0++; // Heartbeat

        if (dspSendDataReady(0))
            dspSendData(0, cmd0);
        if (dspSendDataReady(1))
            dspSendData(1, cmd1);
        if (dspSendDataReady(2))
            dspSendData(2, cmd2);

        if (dspReceiveDataReady(0))
            rep0 = dspReceiveData(0);
        if (dspReceiveDataReady(1))
            rep1 = dspReceiveData(1);
        if (dspReceiveDataReady(2))
            rep2 = dspReceiveData(2);

        printf("REP: %u %d %d\n", rep0, rep1, rep2);
    }

    return 0;
}
