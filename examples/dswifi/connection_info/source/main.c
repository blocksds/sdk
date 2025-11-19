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

    if (!Wifi_InitDefault(WFC_CONNECT | WIFI_ATTEMPT_DSI_MODE))
    {
        printf("Can't connect to WiFi!\n");
    }
    else
    {
        printf("Connected!\n");

        // ASSOCSTATUS_ASSOCIATED is reached if we have an IPv4 or IPv6 address.
        // DHCP for IPv4 is faster than DHCPv6, and this demo focuses on
        // selecting IPv6 or IPv4, so we need to wait for an address to be
        // assigned to us. However, this may never happen if the network doesn't
        // support IPv6, so we can't wait forever.
        //
        // You can remove this wait loop if you want. This is only here so that
        // the example can use IPv6 easier.
        printf("Waiting for an IPv6 address...\n");

        struct in6_addr ipv6;

        unsigned int timeout = 5 * 60; // 5 seconds

        while (1)
        {
            cothread_yield_irq(IRQ_VBLANK);
            if (Wifi_GetIPv6(&ipv6))
                break;

            timeout--;
            if (timeout == 0)
            {
                printf("Can't get IPv6 address\n");
                break;
            }
        }

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
        printf("\n");
        printf("IPv6 information:\n");
        printf("\n");
        char buf[128];
        printf("IP: %s\n", inet_ntop(AF_INET6, &ipv6, buf, sizeof(buf)));
        printf("\n");

        if (Wifi_DisconnectAP() != 0)
            printf("Error: Wifi_DisconnectAP()\n");

        Wifi_DisableWifi();
    }

    printf("\n");
    printf("Press START to exit");

    while (1)
    {
        cothread_yield_irq(IRQ_VBLANK);
        scanKeys();
        if (keysHeld() & KEY_START)
            break;
    }

    return 0;
}
