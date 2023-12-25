// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

// Random file generated with:
//
//     dd bs=1024 count=2048 < /dev/urandom > random.bin

#include <stdio.h>

#include <fatfs.h>
#include <nds.h>
#include <nds/cothread.h>
#include <nds/arm9/dldi.h>

#include "md5/md5.h"

static char input_buffer[1024];

PrintConsole topScreen;
PrintConsole bottomScreen;

int calculate_file_md5(void *arg)
{
    const char *path = arg;

    FILE *f = fopen(path, "r");
    if (f == NULL)
    {
        consoleSelect(&topScreen);
        perror("fopen(random.bin)");
        fflush(stdout);
        return -1;
    }

    consoleSelect(&topScreen);
    printf("Calculating MD5 in a thread\n");
    printf("\n");
    fflush(stdout);

    size_t input_size = 0;

    MD5Context ctx;
    md5Init(&ctx);

    uint32_t size = 0;

    while (1)
    {
        input_size = fread(input_buffer, 1, 1024, f);
        if (input_size <= 0)
            break;

        md5Update(&ctx, (uint8_t *)input_buffer, input_size);

        size += input_size;
        if ((size % (1024 * 64)) == 0)
        {
            consoleSelect(&topScreen);
            printf(".");
            fflush(stdout);
        }
    }

    consoleSelect(&topScreen);
    printf("\n");
    printf("\n");
    printf("\n");
    fflush(stdout);

    md5Finalize(&ctx);

    uint8_t digest[16];
    memcpy(digest, ctx.digest, 16);

    consoleSelect(&topScreen);
    for (int i = 0; i < 16; i++)
        printf("%02X", digest[i]);
    printf("\n");
    fflush(stdout);

    fclose(f);

    return 0;
}

int main(int argc, char **argv)
{
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(&topScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    consoleInit(&bottomScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);

    consoleSelect(&bottomScreen);

    printf("\x1b[2J"); // Clear console

    printf("DLDI name:\n");
    printf("%s\n", io_dldi_data->friendlyName);
    printf("\n");
    printf("DSi mode: %d\n", isDSiMode());

    if (isDSiMode() == 0)
    {
        // In DS mode, access to the SD card is done with DLDI. When running on
        // emulators DLDI is not be needed, but cartridge reads happen in the
        // ARM7 at the moment. DLDI usually runs in the ARM9.
        //
        // If DLDI runs on the ARM9, it isn't possible to do multithreading
        // while accessing the filesystem. That can only work if the ARM7 loads
        // data while the ARM9 waits for it and switches to other threads in the
        // meantime.
        printf("Forcing DLDI in ARM7...\n");
        dldiSetMode(DLDI_MODE_ARM7);
    }

    bool init_ok = nitroFSInit(NULL);
    if (!init_ok)
    {
        perror("nitroFSInit()");
        goto wait_loop;
    }

    chdir("nitro:/");
    char *cwd = getcwd(NULL, 0);
    printf("Current dir: %s\n\n", cwd);
    free(cwd);

    printf("\x1b[10;0;HMain thread: ");
    fflush(stdout);

    // This thread needs enough stack to do filesystem access. By default it
    // isn't enough for it and it will make the ROM crash because of a stack
    // overflow.
    cothread_t load_thread = cothread_create(calculate_file_md5,
                                             (void *)"random.bin", 4 * 1024, 0);

    int count = 0;
    while (1)
    {
        cothread_yield_irq(IRQ_VBLANK);

        consoleSelect(&bottomScreen);
        printf("\x1b[10;14;H%5d", count);
        fflush(stdout);

        count++;

        if (cothread_has_joined(load_thread))
        {
            cothread_delete(load_thread);
            break;
        }
    }

wait_loop:
    printf("\n");
    printf("\n");
    printf("Press START to exit to loader\n");

    while (1)
    {
        cothread_yield_irq(IRQ_VBLANK);

        scanKeys();

        uint32_t keys_down = keysDown();
        if (keys_down & KEY_START)
            break;
    }

    return 0;
}
