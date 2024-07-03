// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <gl2d.h>
#include <filesystem.h>
#include <nds.h>

#include "definitions.h"
#include "graphics.h"
#include "helpers.h"

static invader invaders[NUM_INVADERS];
static float invaders_speed_x = 0.0;
static int invaders_direction_x = 1;

static bullet bullets[NUM_BULLETS];

static float ship_x = 0.0;
static float ship_y = 0.0;

typedef enum
{
    ROOM_START,
    ROOM_GAME,
    ROOM_GAME_OVER,
    ROOM_GAME_WON
}
room_name_t;

room_name_t current_room;

void bullet_shoot(int x, int y)
{
    for (int i = 0; i < NUM_BULLETS; i++)
    {
        if (bullets[i].active)
            continue;

        bullets[i].active = true;
        bullets[i].x = x;
        bullets[i].y = y;
        break;
    }
}

void bullets_update(void)
{
    for (int i = 0; i < NUM_BULLETS; i++)
    {
        if (!bullets[i].active)
            continue;

        bullets[i].y -= BULLET_SPEED;

        // If they leave the screen from the top, delete them
        if (bullets[i].y <= (0 - BULLET_HEIGHT))
            bullets[i].active = false;
    }
}

void invaders_update_speed(void)
{
    int invaders_alive = 0;
    for (int i = 0; i < NUM_INVADERS; i++)
    {
        if (invaders[i].active)
            invaders_alive++;
    }

    int invaders_alive_max = NUM_INVADERS_X * NUM_INVADERS_Y;

    invaders_speed_x = (1.0 * (10 + invaders_alive_max - invaders_alive)) / (invaders_alive + 10);
}

void invaders_update(void)
{
    bool wall_hit = false;

    for (int i = 0; i < NUM_INVADERS; i++)
    {
        if (!invaders[i].active)
            continue;

        // Update animation of the invader
        if (invaders[i].anim_count <= 0)
        {
            invaders[i].anim_count = 127 + (rand() & 255);
            invaders[i].anim_frame ^= 1;
        }
        else
        {
            invaders[i].anim_count--;
        }

        // Move the invader
        invaders[i].x += invaders_speed_x * invaders_direction_x;

        if (invaders[i].x < 4)
            wall_hit = true;
        if ((invaders[i].x + INVADER_WIDTH) > (SCREEN_WIDTH - 4))
            wall_hit = true;

        // Check if a bullet has hit this invader
        for (int j = 0; j < NUM_BULLETS; j++)
        {
            if (!bullets[j].active)
                continue;

            int x1 = invaders[i].x;
            int y1 = invaders[i].y;
            int x2 = x1 + INVADER_WIDTH;
            int y2 = y1 + INVADER_HEIGHT;

            int bx1 = bullets[j].x;
            int by1 = bullets[j].y;
            int bx2 = bx1 + BULLET_WIDTH;
            int by2 = by1 + BULLET_HEIGHT;

            // Check if the two rectangles DON'T overlap. It's easier than
            // checking if they overlap.
            if ((bx2 < x1) || (bx1 > x2) || (by2 < y1) || (by1 > y2))
                continue;

            // Delete both the invader and the bullet
            invaders[i].active = false;
            bullets[j].active = false;

            break;
        }
    }

    invaders_update_speed();

    // If one of the walls has been reached, switch direction and make all
    // invaders drop a bit.
    if (wall_hit)
    {
        invaders_direction_x = -invaders_direction_x;

        for (int i = 0; i < NUM_INVADERS; i++)
        {
            if (!invaders[i].active)
                continue;

            invaders[i].y += 3.0;
        }
    }
}

bool check_game_won(void)
{
    bool won = true;

    for (int i = 0; i < NUM_INVADERS; i++)
    {
        if (invaders[i].active)
        {
            won = false;
            break;
        }
    }

    return won;
}

bool check_game_over(float ship_x, float ship_y)
{
    bool game_over = false;

    for (int i = 0; i < NUM_INVADERS; i++)
    {
        if (!invaders[i].active)
            continue;

        if ((invaders[i].y + INVADER_HEIGHT) >= SCREEN_HEIGHT)
        {
            game_over = true;
        }

        // Check if the invader has hit the ship

        int x1 = invaders[i].x;
        int y1 = invaders[i].y;
        int x2 = x1 + INVADER_WIDTH;
        int y2 = y1 + INVADER_HEIGHT;

        int sx1 = ship_x;
        int sy1 = ship_y;
        int sx2 = sx1 + SHIP_WIDTH;
        int sy2 = sy1 + SHIP_HEIGHT;

        // Check if the two rectangles DON'T overlap. It's easier than
        // checking if they overlap.
        if ((sx2 < x1) || (sx1 > x2) || (sy2 < y1) || (sy1 > y2))
            continue;

        game_over = true;
    }

    return game_over;
}

void invaders_reset(void)
{
    int offset_x = (SCREEN_WIDTH - ((INVADER_WIDTH + SPACE_BETWEEN_INVADERS_X) * NUM_INVADERS_X)) / 2;
    int offset_y = 10;

    for (int j = 0; j < NUM_INVADERS_Y; j++)
    {
        for (int i = 0; i < NUM_INVADERS_X; i++)
        {
            int index = j * NUM_INVADERS_X + i;

            invaders[index].active = true;
            invaders[index].x = offset_x + i * (INVADER_WIDTH + SPACE_BETWEEN_INVADERS_X);
            invaders[index].y = offset_y + j * (INVADER_HEIGHT + SPACE_BETWEEN_INVADERS_Y);
            invaders[index].anim_frame = rand() & 1;
            invaders[index].anim_count = rand() & 255;
        }
    }
}

void bullets_reset(void)
{
    for (int i = 0; i < NUM_BULLETS; i++)
        bullets[i].active = false;
}

void load_room(room_name_t room)
{
    switch (room)
    {
        case ROOM_START:
        {
            current_room = ROOM_START;
            load_sprites();
            ship_x = 0.0;
            ship_y = 0.0;
            load_background("nitro:/graphics/backgrounds/intro_png.grf");
            break;
        }
        case ROOM_GAME:
        {
            current_room = ROOM_GAME;
            invaders_reset();
            bullets_reset();
            ship_x = (SCREEN_WIDTH - SHIP_WIDTH) / 2;
            ship_y = SCREEN_HEIGHT - SHIP_HEIGHT - 1;
            load_background("nitro:/graphics/backgrounds/space_png.grf");
            break;
        }
        case ROOM_GAME_OVER:
        {
            current_room = ROOM_GAME_OVER;
            load_background("nitro:/graphics/backgrounds/game_over_png.grf");
            break;
        }
        case ROOM_GAME_WON:
        {
            current_room = ROOM_GAME_WON;
            load_background("nitro:/graphics/backgrounds/game_won_png.grf");
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    // Setup sub screen to display the debug console
    videoSetModeSub(MODE_0_2D);
    vramSetBankH(VRAM_H_SUB_BG_0x06200000);
    consoleInit(NULL, 0, BgType_Text4bpp, BgSize_T_256x256, 0, 1, false, true);

    printf("Initializing NitroFS...\n");
    if (!nitroFSInit(NULL))
    {
        perror("nitroFSInit()");
        wait_forever();
    }
    printf("NitroFS initialized!\n");

    // Setup the main screen
    videoSetMode(MODE_0_3D);
    vramSetPrimaryBanks(VRAM_A_MAIN_BG, VRAM_B_TEXTURE, VRAM_C_LCD, VRAM_D_LCD);
    vramSetBankE(VRAM_E_TEX_PALETTE);

    // Initialize OpenGL to some sensible defaults
    glScreen2D();

    // Make the rear plane transparent so that the 2D background behind the 3D
    // layer can be seen.
    glClearColor(0, 0, 0, 0);

    load_room(ROOM_START);

    while (1)
    {
        swiWaitForVBlank();

        bgUpdate();

        scanKeys();

        u16 keys_down = keysDown();
        u16 keys_held = keysHeld();

        switch (current_room)
        {
            case ROOM_START:
            {
                consoleClear();

                printf("START: Start game\n");
                printf("\n");
                printf("START+SELECT: Exit to loader\n");

                if (keys_down & KEY_START)
                    load_room(ROOM_GAME);

                if ((keys_held & KEY_START) && (keys_held & KEY_SELECT))
                    return 0;

                break;
            }
            case ROOM_GAME:
            {
                consoleClear();

                printf("PAD:   Move ship\n");
                printf("A:     Shoot\n");
                printf("\n");
                printf("START+SELECT: Exit to loader\n");

                // Handle input
                // ------------

                if (keys_held & KEY_LEFT)
                    ship_x--;
                else if (keys_held & KEY_RIGHT)
                    ship_x++;

                if (keys_down & KEY_A)
                    bullet_shoot(ship_x + SHIP_WIDTH / 2, ship_y - BULLET_HEIGHT);

                if ((keys_held & KEY_START) && (keys_held & KEY_SELECT))
                    return 0;

                // Game logic update
                // -----------------

                bullets_update();
                invaders_update();
                if (check_game_over(ship_x, ship_y))
                    load_room(ROOM_GAME_OVER);

                if (check_game_won())
                    load_room(ROOM_GAME_WON);

                // Render 3D scene
                // ---------------

                // Set up 3D hardware for 2D graphics

                glBegin2D();

                draw_invaders(invaders);

                draw_ship(ship_x, ship_y);

                draw_bullets(bullets);

                glEnd2D();

                glFlush(0);

                break;
            }
            case ROOM_GAME_OVER:
            {
                consoleClear();

                printf("GAME OVER!\n");
                printf("\n");
                printf("START: Start game\n");
                printf("\n");
                printf("START+SELECT: Exit to loader\n");

                if (keys_down & KEY_START)
                    load_room(ROOM_GAME);

                if ((keys_held & KEY_START) && (keys_held & KEY_SELECT))
                    return 0;

                break;
            }
            case ROOM_GAME_WON:
            {
                consoleClear();

                printf("GAME WON!\n");
                printf("\n");
                printf("START: Start game\n");
                printf("\n");
                printf("START+SELECT: Exit to loader\n");

                if (keys_down & KEY_START)
                    load_room(ROOM_GAME);

                if ((keys_held & KEY_START) && (keys_held & KEY_SELECT))
                    return 0;

                break;
            }
        }
    }

    free_graphics();

    return 0;
}
