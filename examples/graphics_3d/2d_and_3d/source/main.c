// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <stdio.h>

#include <nds.h>

// Headers autogenerated for each PNG file inside GFXDIRS in the Makefile
#include "ball.h"
#include "city.h"
#include "neon.h"

int main(int argc, char **argv)
{
    // Enable 3D
    videoSetMode(MODE_0_3D);

    // Setup some VRAM as memory for main engine background, main engine
    // sprites, and 3D textures.
    vramSetPrimaryBanks(VRAM_A_MAIN_BG, VRAM_B_MAIN_SPRITE,
                        VRAM_C_TEXTURE, VRAM_D_TEXTURE);

    // Setup console in the sub screen
    vramSetBankH(VRAM_H_SUB_BG);
    consoleDemoInit();

    // Setup 2D background
    // ===================

    int bg = bgInit(2, BgType_Text8bpp, BgSize_T_256x256, 0, 4);
    bgSetPriority(bg, 2);

    dmaCopy(cityTiles, bgGetGfxPtr(bg), cityTilesLen);
    dmaCopy(cityMap, bgGetMapPtr(bg), cityMapLen);
    dmaCopy(cityPal, BG_PALETTE, cityPalLen);

    // Setup 2D sprites
    // ================

    oamInit(&oamMain, SpriteMapping_1D_32, false);

    oamEnable(&oamMain);

    // Allocate space for the tiles and copy them there
    u16 *gfxMain = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
    dmaCopy(ballTiles, gfxMain, ballTilesLen);

    // Copy palette
    dmaCopy(ballPal, SPRITE_PALETTE, ballPalLen);

    oamSet(&oamMain, 0,
           100, 50, // X, Y
           0, // Priority
           0, // Palette index
           SpriteSize_32x32, SpriteColorFormat_256Color, // Size, format
           gfxMain,  // Graphics offset
           -1, // Affine index
           false, // Double size
           false, // Hide
           false, false, // H flip, V flip
           false); // Mosaic

    oamSet(&oamMain, 1,
           150, 70, // X, Y
           0, // Priority
           0, // Palette index
           SpriteSize_32x32, SpriteColorFormat_256Color, // Size, format
           gfxMain,  // Graphics offset
           -1, // Affine index
           false, // Double size
           false, // Hide
           false, false, // H flip, V flip
           false); // Mosaic

    // Setup 3D
    // ========

    glInit();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ANTIALIAS);

    // IMPORTANT: The 3D background must be transparent (alpha = 0) so that the
    // 2D layers behind it can be seen.
    glClearColor(0, 0, 0, 0);
    glClearPolyID(63);

    glClearDepth(0x7FFF);

    glViewport(0, 0, 255, 191);

    int textureID;

    // Load texture
    glGenTextures(1, &textureID);
    glBindTexture(0, textureID);
    if (glTexImage2D(0, 0, GL_RGBA, 128, 128, 0, TEXGEN_TEXCOORD, neonBitmap) == 0)
    {
        printf("Failed to load texture\n");
        while (1)
            swiWaitForVBlank();
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70, 256.0 / 192.0, 0.1, 40);

    gluLookAt(0.0, 0.0, 2.0,  // Position
              0.0, 0.0, 0.0,  // Look at
              0.0, 1.0, 0.0); // Up

    // Setup done
    // ==========

    int angle_x = 0;
    int angle_z = 0;
    int bg_x = 150, bg_y = 70;
    int spr_x = 150, spr_y = 70;

    while (1)
    {
        // Synchronize game loop to the screen refresh
        swiWaitForVBlank();

        bgSetScroll(bg, bg_x, bg_y);
        bgUpdate();

        oamSetXY(&oamMain, 1, spr_x, spr_y);

        oamUpdate(&oamMain);

        // Print some text in the demo console
        // -----------------------------------

        consoleClear();

        // Print some controls
        printf("R + PAD: Rotate quad\n");
        printf("L + PAD: Scroll background\n");
        printf("X + PAD: Move sprite\n");
        printf("START:   Exit to loader\n");
        printf("\n");

        // Handle user input
        // -----------------

        scanKeys();

        uint16_t keys = keysHeld();

        if (keys & KEY_R)
        {
            if (keys & KEY_LEFT)
                angle_z += 3;
            if (keys & KEY_RIGHT)
                angle_z -= 3;

            if (keys & KEY_UP)
                angle_x += 3;
            if (keys & KEY_DOWN)
                angle_x -= 3;
        }

        if (keys & KEY_L)
        {
            if (keys & KEY_UP)
                bg_y--;
            else if (keys & KEY_DOWN)
                bg_y++;

            if (keys & KEY_LEFT)
                bg_x--;
            else if (keys & KEY_RIGHT)
                bg_x++;
        }

        if (keys & KEY_X)
        {
            if (keys & KEY_UP)
                spr_y--;
            else if (keys & KEY_DOWN)
                spr_y++;

            if (keys & KEY_LEFT)
                spr_x--;
            else if (keys & KEY_RIGHT)
                spr_x++;
        }

        if (keys & KEY_START)
            break;

        // Render 3D scene
        // ---------------

        glMatrixMode(GL_MODELVIEW);

        glPushMatrix();

        glRotateZ(angle_z);
        glRotateX(angle_x);

        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE);

        glBindTexture(0, textureID);

        glColor3f(1, 1, 1);

        glBegin(GL_QUADS);

            GFX_TEX_COORD = (TEXTURE_PACK(0, inttot16(128)));
            glVertex3v16(floattov16(-1), floattov16(-1), 0);

            GFX_TEX_COORD = (TEXTURE_PACK(inttot16(128),inttot16(128)));
            glVertex3v16(floattov16(1), floattov16(-1), 0);

            GFX_TEX_COORD = (TEXTURE_PACK(inttot16(128), 0));
            glVertex3v16(floattov16(1), floattov16(1), 0);

            GFX_TEX_COORD = (TEXTURE_PACK(0,0));
            glVertex3v16(floattov16(-1), floattov16(1), 0);

        glEnd();

        glPopMatrix(1);

        glFlush(0);
    }

    glDeleteTextures(1, &textureID);

    oamFreeGfx(&oamMain, gfxMain);

    return 0;
}

