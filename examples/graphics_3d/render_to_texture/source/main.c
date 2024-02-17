// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008, 2019, 2024

// Render to Texture example

#include <stdio.h>

#include <nds.h>

// This is created automatically from neon.png and neon.grit
#include "neon.h"

// NOTE: You can enable or disable this define. When the define is enabled,
// the demo uses MODE_5_3D. When it isn't, it uses MODE_FB2.
#define USE_3D_MODE

// It uses VRAM banks as follows:
//
// - VRAM_A: Used for regular textures
// - VRAM_B: Used for the rendered texture
// - VRAM_C: Used to capture the final 3D scene
//
// The BGs of the main screen are used as follows:
//
// - If USE_3D_MODE is defined, it alternates between:
//
//   - BG 0: Rendered texture output, hidden under BG 2.
//   - BG 2: Renders the contents of VRAM_C captured during the previous frame.
//
//   and:
//
//   - BG 0: Displays the currently drawn 3D scene, which is captured to VRAM_C.
//
// - If USE_3D_MODE is not defined:
//
//   - MODE_FB2 renders the contents of VRAM_C as a bitmap on the screen. This
//     makes it easier to setup, but it's less flexible as you can't use other
//     layers on top of it.

// Draw a cube face at the specified color
void DrawCube(void)
{
    int bx = floattov16(-0.5);
    int ex = floattov16(0.5);
    int by = floattov16(-0.5);
    int ey = floattov16(0.5);
    int bz = floattov16(-0.5);
    int ez = floattov16(0.5);

    glBegin(GL_QUADS);

        glNormal(NORMAL_PACK(0, 0, floattov10(-0.97)));

        GFX_TEX_COORD = TEXTURE_PACK(0, 0);
        glVertex3v16(bx, by, bz);
        GFX_TEX_COORD = TEXTURE_PACK(0, inttot16(128));
        glVertex3v16(bx, ey, bz);
        GFX_TEX_COORD = TEXTURE_PACK(inttot16(128), inttot16(128));
        glVertex3v16(ex, ey, bz);
        GFX_TEX_COORD = TEXTURE_PACK(inttot16(128), 0);
        glVertex3v16(ex, by, bz);

        glNormal(NORMAL_PACK(0, 0, floattov10(0.97)));

        GFX_TEX_COORD = TEXTURE_PACK(0, 0);
        glVertex3v16(bx, by, ez);
        GFX_TEX_COORD = TEXTURE_PACK(0, inttot16(128));
        glVertex3v16(bx, ey, ez);
        GFX_TEX_COORD = TEXTURE_PACK(inttot16(128), inttot16(128));
        glVertex3v16(ex, ey, ez);
        GFX_TEX_COORD = TEXTURE_PACK(inttot16(128), 0);
        glVertex3v16(ex, by, ez);

        glNormal(NORMAL_PACK(0, floattov10(-0.97), 0));

        GFX_TEX_COORD = TEXTURE_PACK(0, 0);
        glVertex3v16(bx, by, bz);
        GFX_TEX_COORD = TEXTURE_PACK(0, inttot16(128));
        glVertex3v16(bx, by, ez);
        GFX_TEX_COORD = TEXTURE_PACK(inttot16(128), inttot16(128));
        glVertex3v16(ex, by, ez);
        GFX_TEX_COORD = TEXTURE_PACK(inttot16(128), 0);
        glVertex3v16(ex, by, bz);

        glNormal(NORMAL_PACK(0, floattov10(0.97), 0));

        GFX_TEX_COORD = TEXTURE_PACK(0, 0);
        glVertex3v16(bx, ey, bz);
        GFX_TEX_COORD = TEXTURE_PACK(0, inttot16(128));
        glVertex3v16(bx, ey, ez);
        GFX_TEX_COORD = TEXTURE_PACK(inttot16(128), inttot16(128));
        glVertex3v16(ex, ey, ez);
        GFX_TEX_COORD = TEXTURE_PACK(inttot16(128), 0);
        glVertex3v16(ex, ey, bz);

        glNormal(NORMAL_PACK(floattov10(-0.97), 0, 0));

        GFX_TEX_COORD = TEXTURE_PACK(0, 0);
        glVertex3v16(bx, by, bz);
        GFX_TEX_COORD = TEXTURE_PACK(0, inttot16(128));
        glVertex3v16(bx, by, ez);
        GFX_TEX_COORD = TEXTURE_PACK(inttot16(128), inttot16(128));
        glVertex3v16(bx, ey, ez);
        GFX_TEX_COORD = TEXTURE_PACK(inttot16(128), 0);
        glVertex3v16(bx, ey, bz);

        glNormal(NORMAL_PACK(floattov10(0.97), 0, 0));

        GFX_TEX_COORD = TEXTURE_PACK(0, 0);
        glVertex3v16(ex, by, bz);
        GFX_TEX_COORD = TEXTURE_PACK(0, inttot16(128));
        glVertex3v16(ex, by, ez);
        GFX_TEX_COORD = TEXTURE_PACK(inttot16(128), inttot16(128));
        glVertex3v16(ex, ey, ez);
        GFX_TEX_COORD = TEXTURE_PACK(inttot16(128), 0);
        glVertex3v16(ex, ey, bz);

    glEnd();
}

int main(int argc, char *argv[])
{
    int textureID;
    float rotateX = 0.0;
    float rotateY = 0.0;

    powerOn(POWER_ALL);

#ifdef USE_3D_MODE
    videoSetMode(MODE_5_3D | DISPLAY_BG2_ACTIVE);
    REG_BG0CNT |= BG_PRIORITY_1;
    REG_BG2CNT = BG_BMP16_256x256 | BG_PRIORITY_0;
    REG_BG2PA = 1 << 8;
    REG_BG2PB = 0;
    REG_BG2PC = 0;
    REG_BG2PD = 1 << 8;
    REG_BG2X = 0;
    REG_BG2Y = 0;
#else
    videoSetMode(MODE_FB2);
    vramSetBankC(VRAM_C_LCD);
#endif

    videoSetModeSub(MODE_0_2D);
    vramSetBankH(VRAM_H_SUB_BG);

    consoleInit(NULL, 1, BgType_Text4bpp, BgSize_T_256x256, 15, 0, false, true);

    printf("     RTT Demo by AntonioND\n");
    printf("\n");
    printf("    http://www.skylyrac.net\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("A/B:   Scale small cube\n");
    printf("PAD:   Rotate small cube\n");
    printf("START: Exit to loader");
    printf("\x1b[23;0HThanks to DiscoStew. ;)");

    glInit();

    glEnable(GL_TEXTURE_2D | GL_ANTIALIAS);

    // Setup the rear plane. It must have a unique polygon ID for antialias to
    // work.
    glClearPolyID(63);
    glClearDepth(0x7FFF);

    vramSetBankA(VRAM_A_TEXTURE);

    glGenTextures(1, &textureID);
    glBindTexture(0, textureID);
    glTexImage2D(0, 0, GL_RGB, TEXTURE_SIZE_128, TEXTURE_SIZE_128, 0,
                 TEXGEN_TEXCOORD, neonBitmap);

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
    float scale = 1;

    while (1)
    {
        // The video syncronization has to be done before any work is done in a
        // frame. This ensures that emulators and hardware have time to
        // synchronize during the first frame. If not, screens may be switched.
        swiWaitForVBlank();

        scanKeys();
        u16 keys = keysHeld();

        if (keys & KEY_UP)
            rotateX += 3;
        if (keys & KEY_DOWN)
            rotateX -= 3;
        if (keys & KEY_LEFT)
            rotateY += 3;
        if (keys & KEY_RIGHT)
            rotateY -= 3;

        if (keys & KEY_A)
            scale += 0.01;
        if (keys & KEY_B)
            scale -= 0.01;

        if (keys & KEY_START)
            break;

        rx += 1;
        ry += 1;

        if (frame & 1)
        {
            vramSetBankB(VRAM_B_LCD);
#ifdef USE_3D_MODE
            vramSetBankC(VRAM_C_MAIN_BG_0x06000000);
#endif
            REG_DISPCAPCNT =
                // Destination is VRAM_B
                DCAP_BANK(DCAP_BANK_VRAM_B) |
                // Size = 256x192
                DCAP_SIZE(DCAP_SIZE_256x192) |
                // Capture source A only
                DCAP_MODE(DCAP_MODE_A) |
                // Source A = 3D rendered image
                DCAP_SRC_A(DCAP_SRC_A_3DONLY) |
                // Enable capture
                DCAP_ENABLE;

            glViewport(0, 0, 255, 192);

            // Real scene
            // ----------

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(70, 256.0 / 192.0, 0.1, 40);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            // Blue background
            glClearColor(0, 0, 31, 31);

            gluLookAt(0.0, 0.0, 0.6,    // Camera position
                      0.0, 0.0, 0.0,    // Look at
                      0.0, 1.0, 0.0);   // Up

            // Move cube away from the camera
            glTranslate3f32(0, 0, floattof32(-1));

            glRotateX(rx);
            glRotateY(ry);

            // When calling glBindTexture(), the GFX_TEX_FORMAT
            // command is only sent when the new texture isn't
            // currently active. By calling it with an invalid
            // texture we make sure that any future call to
            // glBindTexture() will actually send that command and
            // replace the manual command below.
            glBindTexture(0, -1);

            // The captured texture is 256x192, stored in VRAM_B,
            // and is in RGBA format. It is needed to use 256x256 as
            // size, as only power of two sizes are supported.
            GFX_TEX_FORMAT = (GL_RGBA << 26)
                           | (TEXTURE_SIZE_256 << 20)
                           | (TEXTURE_SIZE_256 << 23)
                           | ((((uintptr_t)VRAM_B) >> 3) & 0xFFFF);

            DrawCube();

            glFlush(0);
        }
        else
        {
            vramSetBankB(VRAM_B_TEXTURE);
#ifdef USE_3D_MODE
            vramSetBankC(VRAM_C_LCD);
#endif
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

            glViewport(0, 64, 128, 192);

            // Texture scene
            // -------------

            glMatrixMode(GL_PROJECTION);

            glLoadIdentity();
            gluPerspective(70, 128 / 128, 0.1, 40);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            // Green background
            glClearColor(0, 31, 0, 31);

            gluLookAt(0.0, 0.0, 1.0,    // Camera position
                      0.0, 0.0, 0.0,    // Look at
                      0.0, 1.0, 0.0);    // Up

            // Move cube away from the camera
            glTranslate3f32(0, 0, floattof32(-1));

            glRotateX(rotateX);
            glRotateY(rotateY);

            glScalef(scale, scale, scale);

            glBindTexture(0, textureID);

            DrawCube();

            glFlush(0);
        }

        frame ^= 1;
    }

    return 0;
}
