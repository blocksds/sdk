// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024-2025

// This demo shows how to connect to access points configured in the firmware or
// how to let the user select one and input the password to connect to it. Then,
// it downloads a website using IPv4 or IPv6 depending on what's available.

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <nds.h>
#include <dswifi9.h>

static Wifi_AccessPoint AccessPoint;

// This function sends an HTTP request to the specified URL and prints the
// response from the server. It tries to use IPv4 or IPv6.
void getHttp(const char *url, const char *path)
{
    printf("Getting address info:\n");

    struct addrinfo hint;

    hint.ai_flags = AI_CANONNAME;
    hint.ai_family = AF_UNSPEC;     // Allow IPv4 and IPv6
    hint.ai_socktype = SOCK_STREAM; // TCP
    hint.ai_protocol = 0;
    hint.ai_addrlen = 0;
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_next = NULL;

    struct addrinfo *result, *rp;

    int err = getaddrinfo(url, "80", &hint, &result);
    if (err != 0)
    {
        printf("getaddrinfo(): %d\n", err);
        return;
    }

    struct addrinfo *found_rp = NULL;

    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        struct sockaddr_in *sinp;
        const char *addr;
        char buf[1024];

        printf("- Canonical Name:\n  %s\n", rp->ai_canonname);
        if (rp->ai_family == AF_INET)
        {
            // This should never happen because we have explicitly asked for
            // IPv6 addresses, but let's keep it here for reference.
            printf("- AF_INET\n");

            sinp = (struct sockaddr_in *)rp->ai_addr;
            addr = inet_ntop(AF_INET, &sinp->sin_addr, buf, sizeof(buf));

            printf("  %s:%d\n", addr, ntohs(sinp->sin_port));

            found_rp = rp;
            break;
        }
        else if (rp->ai_family == AF_INET6)
        {
            printf("- AF_INET6\n");

            sinp = (struct sockaddr_in *)rp->ai_addr;
            addr = inet_ntop(AF_INET6, &sinp->sin_addr, buf, sizeof(buf));

            printf("  [%s]:%d\n", addr, ntohs(sinp->sin_port));

            found_rp = rp;
            break;
        }
    }

    if (found_rp == NULL)
    {
        printf("Can't find IP info!\n");
        freeaddrinfo(result);
        return;
    }

    printf("IP info found!\n");

    int sfd = socket(found_rp->ai_family, found_rp->ai_socktype, found_rp->ai_protocol);
    if (sfd == -1)
    {
        perror("socket");
        freeaddrinfo(result);
        return;
    }

    printf("Socket created!\n");

    if (connect(sfd, found_rp->ai_addr, found_rp->ai_addrlen) == -1)
    {
        perror("connect");
        close(sfd);
        freeaddrinfo(result);
        return;
    }

    freeaddrinfo(result);

    printf("Connected to server!\n");

    static char request_text[1024];

    snprintf(request_text, sizeof(request_text),
        "GET %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "User-Agent: Nintendo DS\r\n\r\n",
        path, url);

    // send our request
    if (write(sfd, request_text, strlen(request_text)) == -1)
    {
        perror("write()");
        close(sfd);
        return;
    }

    printf("Sent our request!\n");

    // Put the socket in non-blocking mode:
    int opt = 1;
    int rc = ioctl(sfd, FIONBIO, (char *)&opt);
    if (rc < 0)
    {
        perror("ioctl()");
        close(sfd);
        return;
    }

    // Print incoming data
    printf("Printing incoming data:\n");

    // 512 KB buffer for the received website
    static char response_buffer[512 * 1024];
    response_buffer[0] = '\0';
    int response_buffer_ptr = 0;

    // Used to know when to stop receiving data
    int content_length = -1;
    int content_end_ptr = -1;

    // Read at most 100 bytes per call so that we can exercise the logic.
    const int chunk_size = 100;

    while (1)
    {
        int recvd_len = read(sfd, &(response_buffer[response_buffer_ptr]),
                             chunk_size);

        if (recvd_len > 0)
        {
            printf("Received %d bytes\n", recvd_len);
            // Some data has been received
            response_buffer_ptr += recvd_len;
            response_buffer[response_buffer_ptr] = '\0'; // NULL-terminate
        }
        else if (recvd_len == 0)
        {
            // The socket has been closed.
            printf("Other side closed connection!\n");
            break;
        }

        // Try to determine the length of the response
        if (content_length == -1)
        {
            const char *searchstr = "Content-Length: ";
            char *start = strstr(response_buffer, searchstr);
            if (start != NULL)
            {
                start += strlen(searchstr);
                int tmp = -1;
                if (sscanf(start, "%d\r\n", &tmp) == 1)
                {
                    content_length = tmp;
                    printf("Read %s%d\n", searchstr, content_length);
                }
            }
        }

        // Try to determine the end of the content
        if ((content_length != -1) && (content_end_ptr == -1))
        {
            // The start of the content comes after an empty line
            const char *searchstr = "\r\n\r\n";
            char *start = strstr(response_buffer, searchstr);
            if (start != NULL)
            {
                int content_start_ptr = start - &response_buffer[0] + strlen(searchstr);
                content_end_ptr = content_start_ptr + content_length;
                printf("Determined end of the content\n");
            }
        }

        // If we know the end of the message, check if we have reached it
        if (content_end_ptr != -1)
        {
            if (content_end_ptr <= response_buffer_ptr)
            {
                printf("Reached end of the message\n");
                break;
            }
        }

        if (response_buffer_ptr + chunk_size >= sizeof(response_buffer))
        {
            printf("Website too big!\n");
            break;
        }

        // When using non-blocking sockets we need to give the other threads a
        // chance to use the CPU.
        cothread_yield();
    }

    // It's good practice to shutdown the socket.
    if (shutdown(sfd, 0) != 0)
    {
        perror("shutdown()");
    }

    // Remove the socket.
    if (close(sfd) != 0)
    {
        perror("close()");
    }

    // Print response (clamp it if it's too long)
    printf("----------\n");
    if (response_buffer_ptr > 500)
        printf("%.*s [...]\n", 500, response_buffer);
    else
        printf("%s\n", response_buffer);
    printf("---------- (%d bytes)\n", response_buffer_ptr);

    printf("Website: %s%s", url, path);
}

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
                AccessPoint = ap;
        }

        if (keys & KEY_A)
        {
            if (AccessPoint.flags & WFLAG_APDATA_COMPATIBLE)
                break;
        }
    }
}

void connect_to_firmware_access_points(void)
{
    printf("Connecting to firmware APs...\n");

    // Autoconnect to firmware access points
    Wifi_AutoConnect();

    // IP settings have been loaded from flash
}

void connect_to_other_access_points(void)
{
    // Search for all available access points
    access_point_selection_menu();

    // Setting everything to 0 will make DHCP determine the IP address
    Wifi_SetIP(0, 0, 0, 0, 0);

    // If the access point requires a password, ask the user to provide it
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
    consoleArm7Setup(&topScreen, 1024); // Redirect ARM7 messages to the console

    printf("Initializing WiFi...\n");

    // If the ROM is loaded without holding L DSWiFi will try to boot in DSi
    // mode. If the user holds L at boot it will force DS mode even on DSi.
    scanKeys();
    u32 flags = INIT_ONLY |
                ((keysHeld() & KEY_L) ? WIFI_DS_MODE_ONLY : WIFI_ATTEMPT_DSI_MODE);

    if (!Wifi_InitDefault(flags))
    {
        printf("Can't initialize WiFi!\n");
        goto end;
    }

    printf("WiFi initialized!\n");
    printf("\n");

    while (1)
    {
connect:
        Wifi_EnableWifi();

        consoleSelect(&bottomScreen);
        consoleClear();
        printf("WiFi connection options:\n");
        printf("\n");
        printf("A: Connect to firmware AP\n");
        printf("B: Search for APs\n");
        printf("\n");
        printf("\n");

        int num_wfc_caps = Wifi_GetData(WIFIGETDATA_NUMWFCAPS, 0, NULL);

        printf("APs configured in firmware: %d\n", num_wfc_caps);
        if (num_wfc_caps <= 0)
            printf("No APs setup: Option A will fail\n");

        int selection = 0;

        while (selection == 0)
        {
            cothread_yield_irq(IRQ_VBLANK);
            scanKeys();
            if (keysDown() & KEY_A)
                selection = 1;
            if (keysDown() & KEY_B)
                selection = 2;
        }

        consoleClear();

        if (selection == 1)
        {
            consoleSelect(&bottomScreen);

            connect_to_firmware_access_points();
        }
        else if (selection == 2)
        {
            consoleSelect(&bottomScreen);

            connect_to_other_access_points();
        }

        consoleSelect(&topScreen);

        consoleSelect(&bottomScreen);

        consoleClear();

        // Wait until we're connected

        printf("Connecting to AP\n");
        printf("Press B to cancel\n");
        printf("\n");

        int oldstatus = -1;
        while (1)
        {
            cothread_yield_irq(IRQ_VBLANK);

            scanKeys();
            if (keysDown() & KEY_B)
                goto connect;

            //consoleClear();
            int status = Wifi_AssocStatus();

            if (status != oldstatus)
            {
                printf("%s\n", ASSOCSTATUS_STRINGS[status]);
                oldstatus = status;
            }

            if (status == ASSOCSTATUS_CANNOTCONNECT)
            {
                printf("\n");
                printf("Cannot connect to AP\n");
                printf("Press START to restart\n");

                while (1)
                {
                    cothread_yield_irq(IRQ_VBLANK);
                    scanKeys();
                    if (keysDown() & KEY_START)
                        goto connect;
                }
            }

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
        char buf[128];
        printf("IP: %s\n", inet_ntop(AF_INET6, &ipv6, buf, sizeof(buf)));
        printf("\n");
        printf("Press A to fetch a website\n");

        while (1)
        {
            cothread_yield_irq(IRQ_VBLANK);

            scanKeys();
            if (keysHeld() & KEY_A)
                break;
        }

        consoleSelect(&bottomScreen);

        getHttp("www.akkit.org", "/dswifi/example1.php");

        consoleSelect(&topScreen);

        if (Wifi_DisconnectAP() != 0)
            printf("Error: Wifi_DisconnectAP()\n");

        consoleSelect(&topScreen);

        printf("\n");
        printf("Press A to restart\n");
        printf("Press B to end demo\n");

        while (1)
        {
            cothread_yield_irq(IRQ_VBLANK);

            scanKeys();
            if (keysDown() & KEY_A)
                break;
            if (keysDown() & KEY_B)
            {
                Wifi_DisableWifi();
                goto end;
            }
        }
    }

end:
    consoleSelect(&topScreen);
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
