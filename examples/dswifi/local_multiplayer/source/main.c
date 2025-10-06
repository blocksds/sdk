// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024-2025

// This example shows how to use CMD/REPLY commands in an application using
// local multiplayer communications.

#include <stdio.h>

#include <nds.h>
#include <dswifi9.h>

static PrintConsole topScreen;
static PrintConsole bottomScreen;

static Wifi_AccessPoint AccessPoint;

// Global game state
// =================

#define MAX_CLIENTS 7

typedef struct {
    u8 x, y;
} player_info;

typedef struct {
    // This is set to 1 when the game has started, it's kept as 0 while the host
    // waits for new clients to be connected.
    u8 has_started;

    // Bit 0 is set to 1 when the host is connected (always). Bits 1 to 7 are
    // set to 1 if a client with that AID is connected. The maximum number of
    // clients allowed is MAX_CLIENTS (7).
    u8 player_mask;

    // Information of all Clients plus host
    player_info player[WIFI_MAX_MULTIPLAYER_CLIENTS + 1];
} game_info;

volatile game_info game;

void ResetGameState(void)
{
    game.has_started = 0;
    game.player_mask = BIT(0); // The host is always enabled

    for (int i = 0; i < WIFI_MAX_MULTIPLAYER_CLIENTS + 1; i++)
    {
        game.player[i].x = 255;
        game.player[i].y = 255;
    }
}

void RenderGameState(void)
{
    for (int i = 0; i < WIFI_MAX_MULTIPLAYER_CLIENTS + 1; i++)
    {
        if (!(game.player_mask & BIT(i)))
            continue;

        if ((game.player[i].x == 255) && (game.player[i].y == 255))
            continue;

        consoleSetCursor(NULL, game.player[i].x / 8, game.player[i].y / 8);
        printf("%d", i);
    }
}

// Host to client packets
// ======================

typedef struct {
    u8 has_started;
    u8 player_mask;
    player_info player[WIFI_MAX_MULTIPLAYER_CLIENTS + 1];
} pkt_host_to_client;

void SendHostStateToClients(void)
{
    pkt_host_to_client host_packet;

    host_packet.has_started = game.has_started;
    host_packet.player_mask = game.player_mask;

    for (int i = 0; i < WIFI_MAX_MULTIPLAYER_CLIENTS + 1; i++)
    {
        if (game.player_mask & BIT(i))
        {
            host_packet.player[i].x = game.player[i].x;
            host_packet.player[i].y = game.player[i].y;
        }
        else
        {
            host_packet.player[i].x = 255;
            host_packet.player[i].y = 255;
        }
    }

    Wifi_MultiplayerHostCmdTxFrame(&host_packet, sizeof(host_packet));
}

void FromHostPacketHandler(Wifi_MPPacketType type, int base, int len)
{
    if (len < sizeof(pkt_host_to_client))
    {
        // TODO: This shouldn't have happened!
        return;
    }

    if (type != WIFI_MPTYPE_CMD)
        return;

    // Save information received from the client into the global state struct
    pkt_host_to_client packet;
    Wifi_RxRawReadPacket(base, sizeof(packet), (void *)&packet);

    for (int i = 0; i < WIFI_MAX_MULTIPLAYER_CLIENTS + 1; i++)
    {
        game.player[i].x = packet.player[i].x;
        game.player[i].y = packet.player[i].y;
    }

    game.player_mask = packet.player_mask;
    game.has_started = packet.has_started;
}

// Client to host packets
// ======================

typedef struct {
    u8 x, y;
} pkt_client_to_host;

void FromClientPacketHandler(Wifi_MPPacketType type, int aid, int base, int len)
{
    if (len < sizeof(pkt_client_to_host))
    {
        // TODO: This shouldn't have happened!
        return;
    }

    if (type != WIFI_MPTYPE_REPLY)
        return;

    // Save information received from the client into the global state struct
    pkt_client_to_host packet;
    Wifi_RxRawReadPacket(base, sizeof(packet), (void *)&packet);

    game.player[aid].x = packet.x;
    game.player[aid].y = packet.y;

    game.player_mask |= BIT(aid);
}

// Game code
// =========

void HostMode(void)
{
    consoleSelect(&topScreen);

    Wifi_MultiplayerHostMode(MAX_CLIENTS, sizeof(pkt_host_to_client),
                             sizeof(pkt_client_to_host));

    ResetGameState();

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
    consoleClear();

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        u16 keys_down = keysDown();

        consoleClear();
        printf("RIGHT: Channel 11\n");
        printf("LEFT:  Channel 1\n");
        printf("A:     Start game\n");
        printf("\n");

        if (keys_down & KEY_RIGHT)
            Wifi_SetChannel(11);
        if (keys_down & KEY_LEFT)
            Wifi_SetChannel(1);

        if (keys_down & KEY_A)
            break;

        int num_clients = Wifi_MultiplayerGetNumClients();
        u16 players_mask = Wifi_MultiplayerGetClientMask();
        printf("Num clients: %d (mask 0x%02X)\n", num_clients, players_mask);
        printf("\n");

        // Print all client information. This normally isn't needed, all you
        // need is the mask of AIDs.
        Wifi_ConnectedClient client[15];
        num_clients = Wifi_MultiplayerGetClients(15, &(client[0]));

        for (int i = 0; i < num_clients; i++)
        {
            printf("%d (%d) %04X:%04X:%04X\n", client[i].association_id,
                   client[i].state, client[i].macaddr[0], client[i].macaddr[1],
                   client[i].macaddr[2]);
        }
    }

    // The game is starting now, prevent new users from connecting
    Wifi_MultiplayerAllowNewClients(false);
    game.has_started = 1;

    while (1)
    {
        swiWaitForVBlank();

        SendHostStateToClients();

        consoleClear();

        printf("START: Leave host mode\n");
        printf("R:     Kick client with AID 1\n");
        printf("TOUCH: Move object\n");
        printf("\n");

        int num_clients = Wifi_MultiplayerGetNumClients();
        u16 players_mask = Wifi_MultiplayerGetClientMask();
        printf("Num clients: %d (mask 0x%02X)\n", num_clients, players_mask);
        printf("\n");

        RenderGameState();

        scanKeys();

        u16 keys_down = keysDown();
        u16 keys_held = keysHeld();

        if (keys_down & KEY_START)
            break;

        if (keys_held & KEY_TOUCH)
        {
            touchPosition touch_pos;
            touchRead(&touch_pos);

            game.player[0].x = touch_pos.px;
            game.player[0].y = touch_pos.py;
        }

        if (keys_down & KEY_R)
            Wifi_MultiplayerKickClientByAID(1);
    }

    Wifi_IdleMode();

    consoleSelect(&bottomScreen);
    consoleClear();

    consoleSelect(&topScreen);
    printf("Left host mode\n");
}

bool AccessPointSelectionMenu(void)
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

            // In multiplayer client mode DSWiFi ignores all access points that
            // don't offer any Nintendo information. Also, DSWiFi host access
            // points don't use any encryption.

            if (ap.nintendo.allows_connections)
                consoleSetColor(&bottomScreen, CONSOLE_DEFAULT);
            else
                consoleSetColor(&bottomScreen, CONSOLE_LIGHT_RED);

            printf("%s [%.24s]\n", i == chosen ? "->" : "  ", ap.ssid);
            printf("   Name: [%.19s]\n", name);
            printf("   Players %d/%d | %08X\n", ap.nintendo.players_current,
                   ap.nintendo.players_max, (unsigned int)ap.nintendo.game_id);
            printf("   %-4s | Ch %2d | RSSI %d\n",
                   Wifi_ApSecurityTypeString(ap.security_type), ap.channel,
                   ap.rssi);
            printf("\n");

            if (i == chosen)
                AccessPoint = ap;

            consoleSetColor(&bottomScreen, CONSOLE_DEFAULT);
        }

        if (keys & KEY_A)
            return true;
    }

    consoleClear();
}

void ClientMode(void)
{
    printf("Start client mode\n");

connect:
    if (!AccessPointSelectionMenu())
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

    printf("Connected to host!\n");

    ResetGameState();

    consoleSelect(&bottomScreen);
    consoleClear();

    printf("Waiting for the game to start\n");
    printf("START: Disconnect\n");

    while (game.has_started == 0)
    {
        swiWaitForVBlank();

        scanKeys();

        u16 keys_down = keysDown();
        if (keys_down & KEY_START)
            goto end;
    }

    consoleSelect(&topScreen);
    printf("Game started!\n");

    consoleSelect(&bottomScreen);

    while (1)
    {
        swiWaitForVBlank();

        consoleClear();
        printf("START: Disconnect\n");
        printf("TOUCH: Move object\n");

        RenderGameState();

        scanKeys();

        u16 keys_down = keysDown();
        u16 keys_held = keysHeld();

        if (keys_down & KEY_START)
            break;

        if (keys_held & KEY_TOUCH)
        {
            // Prepare new packet to be sent to the host with the current touch
            // screen coordinates.

            touchPosition touch_pos;
            touchRead(&touch_pos);

            pkt_client_to_host packet;
            packet.x = touch_pos.px;
            packet.y = touch_pos.py;

            Wifi_MultiplayerClientReplyTxFrame(&packet, sizeof(packet));
        }
    }

end:
    Wifi_IdleMode();

    consoleSelect(&topScreen);
    printf("Left client mode\n");
}

int main(int argc, char *argv[])
{
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(&topScreen, 0, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    consoleInit(&bottomScreen, 0, BgType_Text4bpp, BgSize_T_256x256, 31, 3, false, true);

    consoleSelect(&topScreen);

    consoleArm7Setup(&topScreen, 1024);

    printf("Initializing WiFi...\n");

    if (!Wifi_InitDefault(INIT_ONLY | WIFI_LOCAL_ONLY))
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
                HostMode();
                break;
            }
            if (keys & KEY_Y)
            {
                ClientMode();
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
