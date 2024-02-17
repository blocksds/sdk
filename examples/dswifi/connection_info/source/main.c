// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#include <nds.h>
#include <dswifi9.h>

int main(int argc, char *argv[])
{
    consoleDemoInit();

    printf("Connecting to WiFi using\n");
    printf("firmware settings...\n");

    if (!Wifi_InitDefault(WFC_CONNECT))
    {
        printf("Can't connect to WiFi!\n");
    }
    else
    {
        printf("Connected!\n");

        struct in_addr ip, gateway, mask, dns1, dns2;
        ip = Wifi_GetIPInfo(&gateway, &mask, &dns1, &dns2);

        printf("\n");
        printf("Connection information:\n");
        printf("\n");
        printf("IP:      %s\n", inet_ntoa(ip));
        printf("Gateway: %s\n", inet_ntoa(gateway));
        printf("Mask:    %s\n", inet_ntoa(mask));
        printf("DNS1:    %s\n", inet_ntoa(dns1));
        printf("DNS2:    %s\n", inet_ntoa(dns2));

        if (Wifi_DisconnectAP() != 0)
            printf("Error: Wifi_DisconnectAP()\n");

        Wifi_DisableWifi();
    }

    printf("\n");
    printf("Press START to exit");

    while (1)
    {
        swiWaitForVBlank();
        scanKeys();
        if (keysHeld() & KEY_START)
            break;
    }

    return 0;
}
