// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024-2025

// This example shows how to download a website using IPv4 using alternate
// socket functions like recv() instead of read(), send() instead of write(),
// and closesocket() instead of close().

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
// response from the server.
void getHttp(const char *url, const char *path)
{
    static char request_text[1024];

    snprintf(request_text, sizeof(request_text),
        "GET %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "User-Agent: Nintendo DS\r\n\r\n",
        path, url);

    // Get host information, including the IP address
    struct hostent *myhost = gethostbyname(url);
    if (myhost == NULL)
    {
        perror("gethostbyname()");
        return;
    }

    unsigned long ip = *((unsigned long *)(myhost->h_addr_list[0]));

    printf("IP Address: %ld.%ld.%ld.%ld\n", (ip >> 0) & 0xFF, (ip >> 8) & 0xFF,
           (ip >> 16) & 0xFF, (ip >> 24) & 0xFF);

    // Create a TCP socket
    int my_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (my_socket == -1)
    {
        perror("socket()");
        return;
    }

    printf("Created Socket!\n");

    // Tell the socket to connect to the IP address we found, on port 80 (HTTP)
    struct sockaddr_in sain;
    sain.sin_family = AF_INET;
    sain.sin_port = htons(80);
    sain.sin_addr.s_addr = ip;
    if (connect(my_socket, (struct sockaddr *)&sain, sizeof(sain)) == -1)
    {
        perror("connect()");
        closesocket(my_socket);
        return;
    }

    printf("Connected to server!\n");

    // send our request
    if (send(my_socket, request_text, strlen(request_text), 0) == -1)
    {
        perror("send()");
        closesocket(my_socket);
        return;
    }

    printf("Sent our request!\n");

    // Print incoming data
    printf("Printing incoming data:\n");

    // Put the socket in non-blocking mode:
    int opt = 1;
    int rc = ioctl(my_socket, FIONBIO, (char *)&opt);
    if (rc < 0)
    {
        perror("ioctl()");
        closesocket(my_socket);
        return;
    }

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
        int recvd_len = recv(my_socket, &(response_buffer[response_buffer_ptr]),
                             chunk_size, 0);

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

        cothread_yield();
    }

    // It's good practice to shutdown the socket.
    if (shutdown(my_socket, 0) != 0)
    {
        perror("shutdown()");
    }

    // Remove the socket.
    if (closesocket(my_socket) != 0)
    {
        perror("closesocket()");
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

    printf("Initializing WiFi...\n");

    if (!Wifi_InitDefault(INIT_ONLY | WIFI_ATTEMPT_DSI_MODE))
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

        consoleSelect(&topScreen);

        struct in_addr ip = { 0 }, gateway = { 0 }, mask = { 0 };
        struct in_addr dns1 = { 0 }, dns2 = { 0 };
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
