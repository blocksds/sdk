// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

// This example shows how to implement motion blur using the video capture
// circuit. It takes the 3D output of the GPU, blends it with the contents in
// VRAM D, and saves the output in VRAM D. The main engine has been setup in
// direct VRAM D rendering mode, so only one VRAM bank needs to be used to
// achieve this effect.
//
// Every frame is the result of the current 3D output plus the previously
// displayed output. This means that the blur effect is added on top of the blur
// of the previous frame. If EVB is too big, the blur from the previous frames
// will never completely disappear.

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

int textureID;
float rx = 30, ry = 0;
float rotateX = 45.0;
float rotateY = 45.0;

void DrawScene(void)
{
    gluLookAt(0.0, 0.0, 1.0,    // Camera position
              0.0, 0.0, 0.0,    // Look at
              0.0, 1.0, 0.0);   // Up

    glPushMatrix();
    {
        // Move cube left and away from the camera
        glTranslatef32(floattof32(-0.5), 0, floattof32(-1));

        glRotateX(rx);
        glRotateY(ry);

        glBindTexture(0, textureID);

        DrawTexturedCube();
    }
    glPopMatrix(1);

    glPushMatrix();
    {
        // Move cube right and away from the camera
        glTranslatef32(floattof32(0.5), 0, floattof32(-1));

        glRotateX(rotateX);
        glRotateY(rotateY);

        // Deactivate texture
        glBindTexture(0, 0);

        DrawGouraudCube();
    }
    glPopMatrix(1);
}

int main(int argc, char *argv[])
{
    powerOn(POWER_ALL);

    // Setup the main screen output to be the contents of VRAM_D
    videoSetMode(MODE_VRAM_D);

    vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankD(VRAM_D_LCD);

    consoleDemoInit();

    glInit();

    glEnable(GL_TEXTURE_2D | GL_ANTIALIAS);

    // Setup the rear plane. It must have a unique polygon ID for antialias to
    // work.
    glClearPolyID(63);
    glClearDepth(0x7FFF);

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

    glViewport(0, 0, 255, 191);

    glClearColor(0, 0, 0, 31);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glFrustumf32(-128 * 3, 128 * 3, 96 * 3, -96 * 3,
                 floattof32(0.1), floattof32(40));

    int eva = 3;
    int evb = 12;

    while (1)
    {
        // The video syncronization has to be done before any work is done in a
        // frame.
        swiWaitForVBlank();

        // This will take the current 3D output, blend it with the image stored
        // in VRAM D, and save the result in VRAM D. The video mode is
        // MODE_VRAM_D, so the contents of VRAM D are displayed directly on the
        // screen. This way we only require one VRAM bank for this effect.

        REG_DISPCAPCNT =
            // Destination is VRAM_D
            DCAP_BANK(DCAP_BANK_VRAM_D) |
            // Size = 256x192
            DCAP_SIZE(DCAP_SIZE_256x192) |
            // Capture sources A and B blended
            DCAP_MODE(DCAP_MODE_BLEND) |
            // Blending factors for sources A and B (max is 16)
            DCAP_A(eva) | DCAP_B(evb) |
            // Source A = 3D rendered image
            DCAP_SRC_A(DCAP_SRC_A_3DONLY) |
            // Source B = VRAM D
            DCAP_SRC_B(DCAP_SRC_B_VRAM) |
            DCAP_SRC_ADDR(DCAP_BANK_VRAM_D) |
            // Enable capture
            DCAP_ENABLE;

        // Handle input

        scanKeys();
        u16 keys = keysDown();

        if (keys & KEY_START)
            break;

        if ((keys & KEY_UP) && (eva < 16))
            eva++;
        if ((keys & KEY_DOWN) && (eva > 0))
            eva--;

        if ((keys & KEY_LEFT) && (evb > 0))
            evb--;
        if ((keys & KEY_RIGHT) && (evb < 16))
            evb++;

        rx += 1;
        ry += 1;

        rotateX += 1;
        rotateY -= 1;

        // Print controls

        consoleClear();
        printf("UP/DOWN:    EVA: %d\n", eva);
        printf("LEFT/RIGHT: EVB: %d\n", evb);
        printf("START:      Exit to loader\n");

        // Draw 3D scene

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        DrawScene();

        glFlush(0);
    }

    glDeleteTextures(1, &textureID);

    return 0;
}
