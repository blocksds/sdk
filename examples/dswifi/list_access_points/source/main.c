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

            printf("%s [%.24s]\n", i == chosen ? "->" : "  ", ap.ssid);
            printf("   %-4s | Ch %2d | RSSI %d\n",
                   Wifi_ApSecurityTypeString(ap.security_type), ap.channel,
                   ap.rssi);
            printf("\n");

            if (i == chosen)
                Wifi_GetAPData(chosen, &AccessPoint);
        }

        if (keys & KEY_A)
        {
            // WPA isn't supported
            if (!(AccessPoint.flags & WFLAG_APDATA_WPA))
                break;
        }
    }
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

    access_point_selection_menu();

    consoleClear();

    consoleSelect(&topScreen);

    printf("Selected network:\n");
    printf("\n");
    printf("%.31s\n", AccessPoint.ssid);
    printf("Security: %s | Ch: %d\n",
           Wifi_ApSecurityTypeString(AccessPoint.security_type),
           AccessPoint.channel);
    printf("\n");

    // Setting everything to 0 will make DHCP determine the IP address
    Wifi_SetIP(0, 0, 0, 0, 0);

    consoleSelect(&bottomScreen);

    if (AccessPoint.flags & WFLAG_APDATA_WEP)
    {
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

            bool valid = (password_len == 13) || (password_len == 5);
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

    consoleSelect(&topScreen);

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

    const char *url = "www.wikipedia.com";

    printf("Resolving IP of:\n");
    printf("\n");
    printf("   %s\n", url);
    printf("\n");

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
