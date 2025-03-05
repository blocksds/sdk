// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

// This example shows how to combine CMD/REPLY multiplayer packets alongisde
// regular data packets of arbitrary size.

#include <stdio.h>

#include <nds.h>
#include <dswifi9.h>

// This is only required when using debug builds of DSWifi on the ARM9. However,
// it only prints sgIP messages, and sgIP isn't used in local multiplayer mode.
void sgIP_dbgprint(char *msg, ...)
{
    // Do nothing
}

static PrintConsole topScreen;
static PrintConsole bottomScreen;

static Wifi_AccessPoint AccessPoint;

typedef struct {
    u32 count;
} pkt_host_to_client;

typedef struct {
    u32 uncount;
} pkt_client_to_host;

pkt_host_to_client host_packet;

void SendMultiplayerCmdFrame(void)
{
    host_packet.count++;
    Wifi_MultiplayerHostCmdTxFrame(&host_packet, sizeof(host_packet));
}

pkt_client_to_host client_packet;

void PrepareMultiplayerReplyFrame(void)
{
    client_packet.uncount--;
    Wifi_MultiplayerClientReplyTxFrame(&client_packet, sizeof(client_packet));
}

void FromHostPacketHandler(Wifi_MPPacketType type, int base, int len)
{
    consoleSelect(&topScreen);

    printf("[H] ");

    if (type == WIFI_MPTYPE_CMD)
    {
        printf("C: ");

        pkt_host_to_client packet;

        if (len == sizeof(packet))
        {
            Wifi_RxRawReadPacket(base, sizeof(packet), &packet);
            printf("%08X", (unsigned int)packet.count);
        }
    }
    else if (type == WIFI_MPTYPE_DATA)
    {
        printf("D: ");

        for (int i = 0; i < len; i += 2)
        {
            u16 data;
            Wifi_RxRawReadPacket(base + i, sizeof(data), &data);

            printf("%c", data & 0xFF);
            if ((len - i) > 1)
                printf("%c", (data >> 8) & 0xFF);
        }
    }

    printf("\n");

    consoleSelect(&bottomScreen);
}

void FromClientPacketHandler(Wifi_MPPacketType type, int aid, int base, int len)
{
    consoleSelect(&topScreen);

    printf("[C %d] ", aid);

    if (type == WIFI_MPTYPE_REPLY)
    {
        printf("C: ");

        pkt_client_to_host packet;

        if (len == sizeof(packet))
        {
            Wifi_RxRawReadPacket(base, sizeof(packet), &packet);
            printf("%08X", (unsigned int)packet.uncount);
        }
    }
    else if (type == WIFI_MPTYPE_DATA)
    {
        printf("D: ");

        for (int i = 0; i < len; i += 2)
        {
            u16 data;
            Wifi_RxRawReadPacket(base + i, sizeof(data), &data);

            printf("%c", data & 0xFF);
            if ((len - i) > 1)
                printf("%c", (data >> 8) & 0xFF);
        }
    }

    printf("\n");

    consoleSelect(&bottomScreen);
}

void host_mode(void)
{
    consoleSelect(&topScreen);

    Wifi_MultiplayerHostMode(15, sizeof(pkt_host_to_client),
                             sizeof(pkt_client_to_host));

    Wifi_MultiplayerFromClientSetPacketHandler(FromClientPacketHandler);

    while (!Wifi_LibraryModeReady())
        swiWaitForVBlank();

    // You can call the next functions before loading a beacon to set up the
    // beacon. They can also be called afterwards.
    Wifi_SetChannel(6);
    Wifi_MultiplayerAllowNewClients(true);

    Wifi_BeaconStart("NintendoDS", 0xCAFEF00D);

    swiWaitForVBlank();
    swiWaitForVBlank();

    printf("Host ready!\n");

    consoleSelect(&bottomScreen);

    while (1)
    {
        swiWaitForVBlank();

        consoleClear();

        printf("RIGHT:   Channel 10\n");
        printf("LEFT:    Channel 1\n");
        printf("B:       Reject new clients\n");
        printf("A:       Allow new clients\n");
        printf("START:   Leave host mode\n");
        printf("R:       Kick AID 1\n");
        printf("DOWN/UP: Send text to client 1\n");
        printf("\n");
        printf("\n");

        {
            int num_clients = Wifi_MultiplayerGetNumClients();
            u16 mask = Wifi_MultiplayerGetClientMask();

            printf("Num clients: %d (%02X)\n", num_clients, mask);
            printf("\n");

            Wifi_ConnectedClient client[15];
            num_clients = Wifi_MultiplayerGetClients(15, &(client[0]));

            for (int i = 0; i < num_clients; i++)
            {
                printf("%d (%d) %04X:%04X:%04X\n", client[i].association_id,
                    client[i].state, client[i].macaddr[0], client[i].macaddr[1],
                    client[i].macaddr[2]);
            }
        }

        scanKeys();

        u16 keys_down = keysDown();

        if (keys_down & KEY_START)
            break;

        if (keys_down & KEY_RIGHT)
            Wifi_SetChannel(10);
        if (keys_down & KEY_LEFT)
            Wifi_SetChannel(1);

        if (keys_down & KEY_B)
            Wifi_MultiplayerAllowNewClients(false);
        if (keys_down & KEY_A)
            Wifi_MultiplayerAllowNewClients(true);

        if (keys_down & KEY_L)
            SendMultiplayerCmdFrame();
        if (keys_down & KEY_R)
            Wifi_MultiplayerKickClientByAID(1);

        if (keys_down & KEY_DOWN)
        {
            const char *str = "Host message";
            Wifi_MultiplayerHostToClientDataTxFrame(1, str, strlen(str));
        }

        if (keys_down & KEY_UP)
        {
            const char *str = "Another host message";
            Wifi_MultiplayerHostToClientDataTxFrame(1, str, strlen(str));
        }
    }

    Wifi_IdleMode();

    consoleSelect(&bottomScreen);
    consoleClear();

    consoleSelect(&topScreen);
    printf("Left host mode\n");
}

bool access_point_selection_menu(void)
{
    consoleSelect(&topScreen);

    Wifi_MultiplayerClientMode(sizeof(pkt_client_to_host));

    while (!Wifi_LibraryModeReady())
        swiWaitForVBlank();

    Wifi_ScanMode();

    int chosen = 0;

    consoleSelect(&bottomScreen);

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();
        uint16_t keys = keysDown();

        if (keys & KEY_B)
            return false;

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

        int first = chosen - 1;
        if (first < 0)
            first = 0;

        int last = first + 3;
        if (last >= count)
            last = count - 1;

        for (int i = first; i <= last; i++)
        {
            Wifi_AccessPoint ap;
            Wifi_GetAPData(i, &ap);

            // Note that the libnds console can only print ASCII characters. If
            // the name uses characters outside of that character set, printf()
            // won't be able to print them.
            char name[10 * 4 + 1];
            int ret = utf16_to_utf8(name, sizeof(name), ap.nintendo.name,
                                    ap.nintendo.name_len * 2);
            if (ret <= 0)
                name[0] = '\0';

            // DSWifi host access points don't use any encryption

            const char *security = "Open";
            if (ap.flags & WFLAG_APDATA_WPA)
                security = "WPA ";
            else if (ap.flags & WFLAG_APDATA_WEP)
                security = "WEP ";

            if (ap.nintendo.allows_connections)
                consoleSetColor(&bottomScreen, CONSOLE_DEFAULT);
            else
                consoleSetColor(&bottomScreen, CONSOLE_LIGHT_RED);

            printf("%s [%.24s] %s\n", i == chosen ? "->" : "  ", ap.ssid,
                ap.flags & WFLAG_APDATA_ADHOC ? "*" : "");
            printf("   Name: [%.19s]\n", name);
            printf("   Players %d/%d | %08X\n", ap.nintendo.players_current,
                   ap.nintendo.players_max, (unsigned int)ap.nintendo.game_id);
            printf("   %s | Ch %2d | RSSI %u\n", security, ap.channel, ap.rssi);
            printf("\n");

            if (i == chosen)
                Wifi_GetAPData(chosen, &AccessPoint);

            consoleSetColor(&bottomScreen, CONSOLE_DEFAULT);
        }

        // Currently, multiplayer networks are open
        if (!(AccessPoint.flags & (WFLAG_APDATA_WPA | WFLAG_APDATA_WEP)))
        {
            if (keys & KEY_A)
                return true;
        }
    }

    consoleClear();
}

void client_mode(void)
{
    printf("Start client mode\n");

connect:
    if (!access_point_selection_menu())
        goto end;

    consoleSelect(&topScreen);

    Wifi_MultiplayerFromHostSetPacketHandler(FromHostPacketHandler);

    printf("Selected network:\n");
    printf("\n");
    printf("%.31s\n", AccessPoint.ssid);
    printf("Channel: %d\n", AccessPoint.channel);
    printf("\n");

    Wifi_ConnectOpenAP(&AccessPoint);

    consoleSelect(&bottomScreen);
    consoleClear();

    printf("Connecting to AP\n");
    printf("Press B to cancel\n");
    printf("\n");

    // Wait until we're connected

    int oldstatus = -1;
    while (1)
    {
        swiWaitForVBlank();

        scanKeys();
        if (keysDown() & KEY_B)
            goto connect;

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
                swiWaitForVBlank();
                scanKeys();
                if (keysDown() & KEY_START)
                    goto connect;
            }
        }

        if (status == ASSOCSTATUS_ASSOCIATED)
            break;
    }

    printf("\n");
    printf("Connected to host!\n");
    printf("\n");
    printf("START: Disconnect\n");
    printf("Y:     Prepare REPLY message\n");
    printf("X:     Send text to host\n");

    while (1)
    {
        swiWaitForVBlank();
        scanKeys();
        u16 keys = keysDown();
        if (keys & KEY_START)
            break;

        if (keys & KEY_Y)
            PrepareMultiplayerReplyFrame();

        if (keys & KEY_X)
        {
            const char *msg = "This is a message";
            Wifi_MultiplayerClientToHostDataTxFrame(msg, strlen(msg));
        }
    }

end:
    Wifi_IdleMode();

    consoleSelect(&topScreen);
    printf("Left client mode\n");
}

int main(int argc, char *argv[])
{
    defaultExceptionHandler();

    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(&topScreen, 0, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    consoleInit(&bottomScreen, 0, BgType_Text4bpp, BgSize_T_256x256, 31, 3, false, true);

    consoleSelect(&topScreen);

    consoleArm7Setup(&topScreen, 1024);

    printf("Initializing WiFi...\n");

    if (!Wifi_InitDefault(INIT_ONLY))
    {
        printf("Can't initialize WiFi!\n");
        goto end;
    }

    printf("WiFi initialized!\n");

    while (1)
    {
        consoleSelect(&bottomScreen);
        consoleClear();

        printf("X: Host mode\n");
        printf("Y: Client mode\n");

        while (1)
        {
            swiWaitForVBlank();
            scanKeys();

            u16 keys = keysDown();

            if (keys & KEY_X)
            {
                host_mode();
                break;
            }
            if (keys & KEY_Y)
            {
                client_mode();
                break;
            }
        }
    }

    Wifi_DisableWifi();

end:

    consoleSelect(&bottomScreen);
    consoleClear();

    printf("End of demo!\n");
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
