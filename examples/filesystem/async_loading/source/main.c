// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023-2024

// Random files generated with:
//
//     dd bs=1024 count=2048 < /dev/urandom > random.bin

// Note: Currently this example only works on emulators, not DSi or flashcards.

#include <stdio.h>

#include <filesystem.h>
#include <nds.h>
#include <nds/arm9/dldi.h>
#include <nds/cothread.h>

#include "md5/md5.h"

static char input_buffer[1024];

typedef struct {
    const char *path;
    PrintConsole *console;
} thread_args;

int calculate_file_md5(void *arg)
{
    thread_args *args = arg;

    const char *path = args->path;
    PrintConsole *con = args->console;

    consoleSelect(con);
    printf("Opening: %s\n", path);

    FILE *f = fopen(path, "r");
    if (f == NULL)
    {
        consoleSelect(con);
        perror("fopen");
        fflush(stdout);
        return -1;
    }

    consoleSelect(con);
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
            consoleSelect(con);
            printf(".");
            fflush(stdout);
        }

        cothread_yield();
    }

    consoleSelect(con);
    printf("\n");
    printf("\n");
    fflush(stdout);

    md5Finalize(&ctx);

    uint8_t digest[16];
    memcpy(digest, ctx.digest, 16);

    printf("Calculated:\n");
    consoleSelect(con);
    for (int i = 0; i < 16; i++)
        printf("%02X", digest[i]);
    fflush(stdout);

    fclose(f);

    return 0;
}

int main(int argc, char **argv)
{
    PrintConsole topScreen;
    PrintConsole bottomScreen;

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
    printf("Current dir: %s\n", cwd);
    free(cwd);
    printf("\n");
    printf("\n");

    // The threads need enough stack to do filesystem access. By default it
    // isn't enough for it and it will make the ROM crash because of a stack
    // overflow.
    size_t stack_size = 4 * 1024;

    thread_args args1 = {
        .path = "random1.bin",
        .console = &topScreen
    };
    thread_args args2 = {
        .path = "random2.bin",
        .console = &bottomScreen
    };
    cothread_t load_thread1 =
                    cothread_create(calculate_file_md5, &args1, stack_size, 0);
    cothread_t load_thread2 =
                    cothread_create(calculate_file_md5, &args2, stack_size, 0);

    // Wait until the two threads are done
    bool joined1 = false;
    bool joined2 = false;
    while (!joined1 || !joined2)
    {
        cothread_yield_irq(IRQ_VBLANK);

        if (cothread_has_joined(load_thread1))
        {
            joined1 = true;
            cothread_delete(load_thread1);
        }
        if (cothread_has_joined(load_thread2))
        {
            joined2 = true;
            cothread_delete(load_thread2);
        }
    }

    consoleSelect(&topScreen);
    printf("Expected:\n");
    printf("BB8E334C5F383F22960E9AC9E01CB558");
    consoleSelect(&bottomScreen);
    printf("Expected:\n");
    printf("7F2B08C3D649F46084D351BFD6607D2F");

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
