// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <nds.h>
#include <dswifi9.h>

static Wifi_AccessPoint AccessPoint;

void access_point_selection_menu(void)
{
    // Set the library in scan mode
    Wifi_ScanMode();

    int chosen = 0;

    while (1)
    {
        cothread_yield_irq(IRQ_VBLANK);

        scanKeys();
        uint16_t keys = keysDown();

        // Get find out how many APs there are in the area
        int count = Wifi_GetNumAP();

        consoleClear();

        printf("Number of AP: %d\n", count);
        printf("\n");

        if (count == 0)
            continue;

        if (keys & KEY_UP)
            chosen--;

        if (keys & KEY_DOWN)
            chosen++;

        if (chosen < 0)
            chosen = 0;
        if (chosen >= count)
            chosen = count - 1;

        int first = chosen - 5;
        if (first < 0)
            first = 0;

        int last = first + 6;
        if (last >= count)
            last = count - 1;

        for (int i = first; i <= last; i++)
        {
            Wifi_AccessPoint ap;
            Wifi_GetAPData(i, &ap);

            printf("%s [%.20s]%s\n", i == chosen ? "->" : "  ", ap.ssid,
                   (ap.flags & WFLAG_APDATA_CONFIG_IN_WFC) ? " WFC" : "");
            printf("   %-4s | Ch %2d | RSSI %d\n",
                   Wifi_ApSecurityTypeString(ap.security_type), ap.channel,
                   ap.rssi);
            printf("\n");

            if (i == chosen)
                AccessPoint = ap;
        }

        if (keys & KEY_A)
        {
            if (AccessPoint.flags & WFLAG_APDATA_COMPATIBLE)
                break;
        }
    }
}

void connect_to_other_access_points(void)
{
    // Search for all available access points
    access_point_selection_menu();

    // Setting everything to 0 will make DHCP determine the IP address
    Wifi_SetIP(0, 0, 0, 0, 0);

    bool wfc_settings_used = false;

    if (AccessPoint.flags & WFLAG_APDATA_CONFIG_IN_WFC)
    {
        // If the AP is known, use the password stored in the WFC settings. Ask
        // the user whether to use the saved settings or to type the password
        // manually,

        printf("WFC settings found:\n");
        printf("\n");
        printf("A: Use WFC password\n");
        printf("B: Type password manually\n");
        printf("\n");

        while (1)
        {
            cothread_yield_irq(IRQ_VBLANK);
            scanKeys();
            u16 keys = keysDown();
            if (keys & KEY_A)
            {
                wfc_settings_used = true;
                printf("Using WFC settings...\n");
                printf("\n");
                Wifi_ConnectWfcAP(&AccessPoint);
                break;
            }
            if (keys & KEY_B)
            {
                printf("Ignoring WFC settings...\n");
                printf("\n");
                break;
            }
        }
    }

    if (!wfc_settings_used)
    {
        // This AP isn't in the WFC settings. If the access point requires a
        // WEP/WPA password, ask the user to provide it. Note that you can still
        // allow the user to call this function with an AP that is saved in the
        // WFC settings, but the function will ignore the saved settings in the
        // WFC configuration and use the provided password instead.
        if (AccessPoint.security_type != AP_SECURITY_OPEN)
        {
            consoleClear();

            char password[100];
            size_t password_len;

            printf("Please, enter the password:\n");

            while (1)
            {
                password[0] = '\0';
                scanf("%s", password);

                password_len = strlen(password);
                if (password[password_len - 1] == '\n')
                    password[password_len - 1] = '\0';

                bool valid = false;
                if (AccessPoint.security_type == AP_SECURITY_WEP)
                    valid = (password_len == 13) || (password_len == 5);
                else
                    valid = (password_len <= 64);

                if (valid)
                    break;

                printf("Invalid key length! [%s] %zu\n", password, password_len);
            }

            Wifi_ConnectSecureAP(&AccessPoint, password, password_len);
        }
        else
        {
            Wifi_ConnectSecureAP(&AccessPoint, NULL, 0);
        }
    }

    printf("Selected network:\n");
    printf("\n");
    printf("%.31s\n", AccessPoint.ssid);
    printf("Security: %s | Ch: %d\n",
           Wifi_ApSecurityTypeString(AccessPoint.security_type),
           AccessPoint.channel);
    printf("\n");
}

// Callback called whenever the keyboard is pressed so that a character is
// printed on the screen.
void on_key_pressed(int key)
{
   if (key > 0)
      printf("%c", key);
}

int main(int argc, char *argv[])
{
    PrintConsole topScreen;
    PrintConsole bottomScreen;

    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(&topScreen, 0, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    consoleInit(&bottomScreen, 0, BgType_Text4bpp, BgSize_T_256x256, 31, 3, false, true);

    // Load demo keyboard
    Keyboard *kbd = keyboardDemoInit();
    kbd->OnKeyPressed = on_key_pressed;

    consoleSelect(&topScreen);

    printf("Initializing WiFi...\n");

    if (!Wifi_InitDefault(INIT_ONLY | WIFI_ATTEMPT_DSI_MODE))
    {
        printf("Can't initialize WiFi!\n");
        goto end;
    }

    printf("WiFi initialized!\n");
    printf("\n");

    consoleSelect(&bottomScreen);

    connect_to_other_access_points();

    consoleClear();

    // Wait until we're connected

    while (1)
    {
        cothread_yield_irq(IRQ_VBLANK);

        consoleClear();
        int status = Wifi_AssocStatus();

        printf("Status: %s\n", ASSOCSTATUS_STRINGS[status]);
        if (status == ASSOCSTATUS_ASSOCIATED)
            break;
    }

    consoleClear();

    // Get network information

    // ASSOCSTATUS_ASSOCIATED is reached if we have an IPv4 or IPv6 address.
    // DHCP for IPv4 is faster than DHCPv6, and this demo focuses on
    // selecting IPv6 or IPv4, so we need to wait for an address to be
    // assigned to us. However, this may never happen if the network doesn't
    // support IPv6, so we can't wait forever.
    //
    // You can remove this wait loop if you want. This is only here so that
    // the example can use IPv6 easier.
    printf("Waiting for an IPv6 address...\n");

    struct in6_addr ipv6 = { 0 };

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

    consoleSelect(&topScreen);

    struct in_addr ip = { 0 }, gateway = { 0 }, mask = { 0 };
    struct in_addr dns1 = { 0 }, dns2 = { 0 };
    ip = Wifi_GetIPInfo(&gateway, &mask, &dns1, &dns2);

    printf("\n");
    printf("IPv4 information:\n");
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

    const char *url = "www.wikipedia.com";

    printf("Resolving IP of:\n");
    printf("\n");
    printf("   %s\n", url);
    printf("\n");

    // Note: Don't use gethostbyname() in new applications, use getaddrinfo()
    // like in other examples. gethostbyname() is only used here because the
    // code is shorter and this example doesn't focus on connecting to a remote
    // server, only on connecting to an access point.
    struct hostent *host = gethostbyname(url);

    if (host)
        printf("IP: %s\n", inet_ntoa(*(struct in_addr *)host->h_addr_list[0]));
    else
        printf("Could not get IP\n");

    if (Wifi_DisconnectAP() != 0)
        printf("Error: Wifi_DisconnectAP()\n");

    Wifi_DisableWifi();

end:
    consoleSelect(&bottomScreen);

    consoleClear();

    printf("End of demo!\n");
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
