// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008, 2019, 2024

#include <stdio.h>

#include <fat.h>
#include <nds.h>

#include "lodepng.h"

// This is created automatically from neon.png and neon.grit
#include "neon.h"

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

    // Main screen setup
    // -----------------

    videoSetMode(MODE_0_3D | DISPLAY_BG1_ACTIVE);

    // Allocate some VRAM to be used for backgrounds in the main engine
    vramSetBankF(VRAM_F_MAIN_BG_0x06000000);

    // Setup layer 1 as console layer
    consoleInit(NULL, 1, BgType_Text4bpp, BgSize_T_256x256, 7, 0, true, true);

    // Set layer 0 (3D layer) to priority 1 (lower priority than 0)
    bgSetPriority(0, 1);

    // Set layer 1 (console) to priority 0
    bgSetPriority(1, 0);

    bool init_ok = fatInitDefault();
    if (!init_ok)
    {
        perror("fatInitDefault()");
        while (1)
            swiWaitForVBlank();
    }

    printf("A:      Capture 3D output\n");
    printf("B:      Capture 3D+2D output\n");
    printf("SELECT: Save to PNG\n");
    printf("START:  Exit to loader\n");

    // Sub screen setup
    // ----------------

    videoSetModeSub(MODE_5_2D | DISPLAY_BG2_ACTIVE);

    bgInitSub(2, BgType_Bmp16, BgSize_B16_256x256, 0, 0);

    // 3D initialization
    //------------------

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

    while (1)
    {
        // The video syncronization has to be done before any work is done in a
        // frame. This ensures that emulators and hardware have time to
        // synchronize during the first frame. If not, screens may be switched.
        swiWaitForVBlank();

        scanKeys();
        u16 keys = keysDown();

        rotateX += 3;
        rotateY += 1;

        if (keys & KEY_START)
            break;

        if (keys & KEY_A)
        {
            // Enable capture for one frame

            vramSetBankC(VRAM_C_LCD);

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
        }
        else if (keys & KEY_B)
        {
            // Enable capture for one frame

            vramSetBankC(VRAM_C_LCD);

            REG_DISPCAPCNT =
                // Destination is VRAM_C
                DCAP_BANK(DCAP_BANK_VRAM_C) |
                // Size = 256x192
                DCAP_SIZE(DCAP_SIZE_256x192) |
                // Capture source A only
                DCAP_MODE(DCAP_MODE_A) |
                // Source A = 3D+2D output
                DCAP_SRC_A(DCAP_SRC_A_COMPOSITED) |
                // Enable capture
                DCAP_ENABLE;
        }
        else if (keys & KEY_SELECT)
        {
            // Enable capture for one frame

            vramSetBankC(VRAM_C_LCD);

            uint8_t *buffer = malloc(sizeof(uint8_t) * 4 * 256 * 192);
            if (buffer == NULL)
            {
                printf("Not enough memory\n");
            }
            else
            {
                // Convert from 16 bit (5R 5G 5B) to 32 bit (8R 8G 8B 8A)
                for (int y = 0; y < 192; y++)
                {
                    for (int x = 0; x < 256; x++)
                    {
                        int index = (y * 256 + x) * 4;

                        uint16_t color = ((uint16_t *)VRAM_C)[y * 256 + x];

                        buffer[index + 0] = ((color >> 0) & 0x1F) << 3;
                        buffer[index + 1] = ((color >> 5) & 0x1F) << 3;
                        buffer[index + 2] = ((color >> 10) & 0x1F) << 3;
                        buffer[index + 3] = 0xFF;
                    }
                }

                unsigned int error = lodepng_encode32_file("screenshot.png",
                                                           buffer, 256, 192);
                if (error)
                    printf("Error %u: %s\n", error, lodepng_error_text(error));
            }

            vramSetBankC(VRAM_C_SUB_BG_0x06200000);
        }
        else
        {
            // Display captured image

            vramSetBankC(VRAM_C_SUB_BG_0x06200000);
        }

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

        glBindTexture(0, textureID);

        glRotateX(rotateX);
        glRotateY(rotateY);

        DrawCube();

        glFlush(0);
    }

    return 0;
}
