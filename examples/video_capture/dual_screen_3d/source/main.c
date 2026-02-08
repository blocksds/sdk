// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

// Dual screen 3D example.
//
// This method of doing dual screen 3D is stable. That means that even if there
// is a framerate drop, the screens won't flicker, like it happens with other
// systems of dual screen 3D. To test this, hold button A to trap the CPU in a
// loop until it is released. The images will stay static, but they will remain
// stable on their right screens.

#include <stdio.h>

#include <nds.h>

// This is created automatically from neon.png and neon.grit
#include "neon.h"

void DrawTexturedCube(void)
{
    int bx = floattov16(-0.5);
    int ex = floattov16(0.5);
    int by = floattov16(-0.5);
    int ey = floattov16(0.5);
    int bz = floattov16(-0.5);
    int ez = floattov16(0.5);

    glBegin(GL_QUADS);

        glNormal(NORMAL_PACK(0, 0, floattov10(-0.97)));

        glTexCoord2t16(0, 0);
        glVertex3v16(bx, by, bz);
        glTexCoord2t16(0, inttot16(128));
        glVertex3v16(bx, ey, bz);
        glTexCoord2t16(inttot16(128), inttot16(128));
        glVertex3v16(ex, ey, bz);
        glTexCoord2t16(inttot16(128), 0);
        glVertex3v16(ex, by, bz);

        glNormal(NORMAL_PACK(0, 0, floattov10(0.97)));

        glTexCoord2t16(0, 0);
        glVertex3v16(bx, by, ez);
        glTexCoord2t16(0, inttot16(128));
        glVertex3v16(bx, ey, ez);
        glTexCoord2t16(inttot16(128), inttot16(128));
        glVertex3v16(ex, ey, ez);
        glTexCoord2t16(inttot16(128), 0);
        glVertex3v16(ex, by, ez);

        glNormal(NORMAL_PACK(0, floattov10(-0.97), 0));

        glTexCoord2t16(0, 0);
        glVertex3v16(bx, by, bz);
        glTexCoord2t16(0, inttot16(128));
        glVertex3v16(bx, by, ez);
        glTexCoord2t16(inttot16(128), inttot16(128));
        glVertex3v16(ex, by, ez);
        glTexCoord2t16(inttot16(128), 0);
        glVertex3v16(ex, by, bz);

        glNormal(NORMAL_PACK(0, floattov10(0.97), 0));

        glTexCoord2t16(0, 0);
        glVertex3v16(bx, ey, bz);
        glTexCoord2t16(0, inttot16(128));
        glVertex3v16(bx, ey, ez);
        glTexCoord2t16(inttot16(128), inttot16(128));
        glVertex3v16(ex, ey, ez);
        glTexCoord2t16(inttot16(128), 0);
        glVertex3v16(ex, ey, bz);

        glNormal(NORMAL_PACK(floattov10(-0.97), 0, 0));

        glTexCoord2t16(0, 0);
        glVertex3v16(bx, by, bz);
        glTexCoord2t16(0, inttot16(128));
        glVertex3v16(bx, by, ez);
        glTexCoord2t16(inttot16(128), inttot16(128));
        glVertex3v16(bx, ey, ez);
        glTexCoord2t16(inttot16(128), 0);
        glVertex3v16(bx, ey, bz);

        glNormal(NORMAL_PACK(floattov10(0.97), 0, 0));

        glTexCoord2t16(0, 0);
        glVertex3v16(ex, by, bz);
        glTexCoord2t16(0, inttot16(128));
        glVertex3v16(ex, by, ez);
        glTexCoord2t16(inttot16(128), inttot16(128));
        glVertex3v16(ex, ey, ez);
        glTexCoord2t16(inttot16(128), 0);
        glVertex3v16(ex, ey, bz);

    glEnd();
}

void DrawGouraudCube(void)
{
    int bx = floattov16(-0.5);
    int ex = floattov16(0.5);
    int by = floattov16(-0.5);
    int ey = floattov16(0.5);
    int bz = floattov16(-0.5);
    int ez = floattov16(0.5);

    glBegin(GL_QUADS);

        glColor(RGB15(31, 0, 0));

        glVertex3v16(bx, by, bz);
        glVertex3v16(bx, ey, bz);
        glVertex3v16(ex, ey, bz);
        glVertex3v16(ex, by, bz);

        glColor(RGB15(0, 31, 0));

        glVertex3v16(bx, by, ez);
        glVertex3v16(bx, ey, ez);
        glVertex3v16(ex, ey, ez);
        glVertex3v16(ex, by, ez);

        glColor(RGB15(0, 0, 31));

        glVertex3v16(bx, by, bz);
        glVertex3v16(bx, by, ez);
        glVertex3v16(ex, by, ez);
        glVertex3v16(ex, by, bz);

        glColor(RGB15(31, 0, 31));

        glVertex3v16(bx, ey, bz);
        glVertex3v16(bx, ey, ez);
        glVertex3v16(ex, ey, ez);
        glVertex3v16(ex, ey, bz);

        glColor(RGB15(31, 31, 0));

        glVertex3v16(bx, by, bz);
        glVertex3v16(bx, by, ez);
        glVertex3v16(bx, ey, ez);
        glVertex3v16(bx, ey, bz);

        glColor(RGB15(0, 31, 31));

        glVertex3v16(ex, by, bz);
        glVertex3v16(ex, by, ez);
        glVertex3v16(ex, ey, ez);
        glVertex3v16(ex, ey, bz);

    glEnd();
}

void SetupSprites(void)
{
    SpriteEntry *Sprites = calloc(128, sizeof(SpriteEntry));
    if (Sprites == NULL)
    {
        consoleDemoInit();
        printf("Failed to allocate memory!");
        while (1)
            swiWaitForVBlank();
    }

    // Setup a grid of bitmap sprites to form a background

    // Reset sprites
    for (int i = 0; i < 128; i++)
        Sprites[i].attribute[0] = ATTR0_DISABLED;

    int i = 0;
    for (int y = 0; y < 3; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            Sprites[i].attribute[0] = ATTR0_BMP | ATTR0_SQUARE | (64 * y);
            Sprites[i].attribute[1] = ATTR1_SIZE_64 | (64 * x);
            Sprites[i].attribute[2] = ATTR2_ALPHA(1) | (8 * 32 * y) | (8 * x);
            i++;
        }
    }

    // Flush the buffer so that we can use a DMA copy
    DC_FlushRange(Sprites, sizeof(SpriteEntry) * 128);

    dmaCopy(Sprites, OAM_SUB, 128 * sizeof(SpriteEntry));

    free(Sprites);
}

int main(int argc, char *argv[])
{
    int textureID;

    powerOn(POWER_ALL);

    videoSetMode(0);

    SetupSprites();

    // Initialize the background layer we will use to display the captured 3D
    // scene on the sub screen
    videoSetModeSub(MODE_5_2D);
    bgInitSub(2, BgType_Bmp16, BgSize_B16_256x256, 0, 0);

    glInit();

    glEnable(GL_TEXTURE_2D | GL_ANTIALIAS);

    // Setup the rear plane. It must have a unique polygon ID for antialias to
    // work.
    glClearPolyID(63);
    glClearDepth(0x7FFF);

    vramSetBankA(VRAM_A_TEXTURE);

    glGenTextures(1, &textureID);
    glBindTexture(0, textureID);
    if (glTexImageNtr2D(GL_RGBA, 128, 128, TEXGEN_TEXCOORD, neonBitmap, NULL) == 0)
    {
        consoleDemoInit();
        printf("Failed to load texture\n");
        while (1)
            swiWaitForVBlank();
    }

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();

    glMaterialf(GL_AMBIENT,  RGB15(5, 5, 5));
    glMaterialf(GL_DIFFUSE,  RGB15(31, 31, 31));
    glMaterialf(GL_SPECULAR, RGB15(0, 0, 0));
    glMaterialf(GL_EMISSION, RGB15(0, 0, 0));

    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0);

    glLight(0, RGB15(31, 31, 31),
            floattov10(-0.6), floattov10(-0.6), floattov10(-0.6));

    int frame = 0;
    float rx = 30, ry = 0;
    float rotateX = 45.0;
    float rotateY = 45.0;

    while (1)
    {
        // The video syncronization has to be done before any work is done in a
        // frame. This ensures that emulators and hardware have time to
        // synchronize during the first frame. If not, screens may be switched.
        swiWaitForVBlank();

        scanKeys();
        u16 keys = keysHeld();

        if (keys & KEY_START)
            break;

        // If the user presses A, wait in a loop until the button is released.
        // This is a test to see that a framerate drop won't cause any issues.
        if (keys & KEY_A)
        {
            while (1)
            {
                swiWaitForVBlank();
                scanKeys();
                if (!(keysHeld() & KEY_A))
                    break;
            }
        }

        rx += 1;
        ry += 1;

        rotateX += 1;
        rotateY -= 1;

        if (frame & 1)
        {
            lcdMainOnTop();

            videoSetMode(MODE_VRAM_C);
            videoSetModeSub(MODE_0_2D | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_2D_BMP_256);

            vramSetBankC(VRAM_C_LCD);
            vramSetBankD(VRAM_D_SUB_SPRITE_0x06600000);

            REG_DISPCAPCNT =
                // Destination is VRAM_C
                DCAP_BANK(DCAP_BANK_VRAM_C) |
                // Size = 256x192
                DCAP_SIZE(DCAP_SIZE_256x192) |
                // Capture source A only
                DCAP_MODE(DCAP_MODE_A) |
                // Source A = 3D rendered image
                DCAP_SRC_A(DCAP_SRC_A_3DONLY) |
                // Enable capture
                DCAP_ENABLE;

            glViewport(0, 0, 255, 191);

            // Scene with Gouraud cube
            // -----------------------

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(70, 256.0 / 192.0, 0.1, 40);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            // Blue background
            glClearColor(7, 20, 7, 31);

            gluLookAt(0.0, 0.0, 1.0,    // Camera position
                      0.0, 0.0, 0.0,    // Look at
                      0.0, 1.0, 0.0);   // Up

            // Move cube away from the camera
            glTranslatef32(0, 0, floattof32(-1));

            glRotateX(rx);
            glRotateY(ry);

            glBindTexture(0, textureID);

            DrawTexturedCube();

            glFlush(0);
        }
        else
        {
            lcdMainOnBottom();

            videoSetMode(MODE_VRAM_D);
            videoSetModeSub(MODE_5_2D | DISPLAY_BG2_ACTIVE);

            vramSetBankC(VRAM_C_SUB_BG_0x06200000);
            vramSetBankD(VRAM_D_LCD);

            REG_DISPCAPCNT =
                // Destination is VRAM_D
                DCAP_BANK(DCAP_BANK_VRAM_D) |
                // Size = 256x192
                DCAP_SIZE(DCAP_SIZE_256x192) |
                // Capture source A only
                DCAP_MODE(DCAP_MODE_A) |
                // Source A = 3D rendered image
                DCAP_SRC_A(DCAP_SRC_A_3DONLY) |
                // Enable capture
                DCAP_ENABLE;

            glViewport(0, 0, 255, 191);

            // Scene without Gouraud cube
            // --------------------------

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(70, 256.0 / 192.0, 0.1, 40);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            // Blue background
            glClearColor(7, 7, 20, 31);

            gluLookAt(0.0, 0.0, 1.0,    // Camera position
                      0.0, 0.0, 0.0,    // Look at
                      0.0, 1.0, 0.0);   // Up

            // Move cube away from the camera
            glTranslatef32(0, 0, floattof32(-1));

            glRotateX(rotateX);
            glRotateY(rotateY);

            // Deactivate texture
            glBindTexture(0, 0);

            DrawGouraudCube();

            glFlush(0);
        }

        frame ^= 1;
    }

    glDeleteTextures(1, &textureID);

    return 0;
}
