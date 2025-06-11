// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <stdio.h>

#include <nds.h>
#include <nds/arm9/dldi.h>

int main(int argc, char **argv)
{
    consoleDemoInit();

    printf("Exit to loader\n");
    printf("--------------\n");
    printf("\n");
    printf("L+R+START+SELECT: ARM7 exit\n");
    printf("A+B:              ARM9 exit\n");
    printf("\n");

    printf("DLDI name:\n");
    printf("%s\n", io_dldi_data->friendlyName);
    printf("\n");
    printf("DSi mode: %d\n", isDSiMode());
    printf("\n");

    extern char *fake_heap_end;
    struct __bootstub *bootstub = __system_bootstub;

    printf("bootstub:      0x%X\n", (uintptr_t)bootstub);
    printf("fake_heap_end: 0x%X\n", (uintptr_t)fake_heap_end);
    printf("\n");
    printf("bootsig: 0x%llX %s\n", bootstub->bootsig,
           bootstub->bootsig == BOOTSIG ? "OK" : "BAD");
    printf("\n");
    printf("arm9reboot: 0x%X\n", (uintptr_t)bootstub->arm9reboot);
    printf("arm7reboot: 0x%X\n", (uintptr_t)bootstub->arm9reboot);
    printf("bootsize:   0x%lX\n", bootstub->bootsize);

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint32_t keys = keysHeld();
        if ((keys & KEY_A) && (keys & KEY_B))
            break;
    }

    return 0;
}
