// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024-2026

// This example shows how to enable the clear bitmap feature of the GPU. When
// enabled, two texture banks are used for it:
//
// - Texture slot 2 (normally VRAM C) contains a 16-bit bitmap with the image to
//   be displayed.
//
// - Texture slot 3 (normally VRAM D) contains a depth bitmap with values from 0
//   (near plane) and 0x7FFFF (far plane).
//
// The depth bitmap is hard to get right. The depth values of your 3D objects
// depend on whether you use Z-buffering or W-buffering, and it also depends on
// your projection matrix (because that matrix configures how the GPU generates
// the Z and W values of your polygons).
//
// This example also shows how to lock VRAM C and D so that libnds doesn't
// allocate textures to them. Normally, libnds allocates textures to any VRAM
// bank used for textures. We need to lock banks C and D so that libnds knows
// textures can't be loaded to them.
//
// In practice, the clear bitmap feature isn't very useful. It's normally easier
// to use 3D polygons to display 2D elements, or to use the 2D hardware to
// display backgrounds behind your 3D scene.

#include <nds.h>

#include "photo.h"
#include "teapot_bin.h"
#include "teapot.h"

int main(int argc, char *argv[])
{
    // Reset main VRAM banks before setting up the console so that we don't
    // write anything to the main VRAM banks. Normally libnds sets up one VRAM
    // bank for sub backgrounds during initialization of the application.
    vramSetPrimaryBanks(VRAM_A_LCD, VRAM_B_LCD, VRAM_C_LCD, VRAM_D_LCD);

    // Setup console on the bottom screen
    videoSetModeSub(MODE_0_2D);
    vramSetBankH(VRAM_H_SUB_BG);
    consoleInit(NULL, 3, BgType_Text4bpp, BgSize_T_256x256, 8, 0, false, true);

    // Initialize 3D
    videoSetMode(MODE_0_3D);

    glInit();

    vramSetBankA(VRAM_A_TEXTURE); // Same as VRAM_A_TEXTURE_SLOT0
    vramSetBankB(VRAM_B_TEXTURE); // Same as VRAM_B_TEXTURE_SLOT1

    vramSetBankC(VRAM_C_LCD);
    vramSetBankD(VRAM_D_LCD);

    // Load clear bitmap
    memcpy(VRAM_C, photoBitmap, photoBitmapLen);

    // Load depth bitmap.
    for (int j = 0; j < 192; j++)
    {
        for (int i = 0; i < 256; i++)
        {
            // This code creates a very simple pattern of a checkered board with
            // squares that have the minimum depth and the maximum depth to show
            // that the system works.
            if ((i & 64) ^ (j & 64))
                VRAM_D[(j * 256) + i] = 0;
            else
                VRAM_D[(j * 256) + i] = 0x7FFF;
        }
    }

    vramSetBankC(VRAM_C_TEXTURE_SLOT2); // Slot 2 contains the bitmap
    vramSetBankD(VRAM_D_TEXTURE_SLOT3); // Slot 3 contains the depth map

    // Tell libnds to not use VRAM C or D when loading textures
    glLockVRAMBank(VRAM_C);
    glLockVRAMBank(VRAM_D);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ANTIALIAS);

    // Setup the rear plane
    glClearColor(0, 0, 0, 0); // Set BG to black
    // The BG and polygons will have the same ID unless a polygon is highlighted
    glClearPolyID(0);
    // When using a clear bitmap the value of glClearDepth() is ignored
    glClearDepth(0x7FFF);

    int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(0, textureID);
    if (glTexImageNtr2D(GL_RGBA, 256, 256,
                        GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T,
                        teapotBitmap, NULL) == 0)
    {
        printf("Failed to load texture\n");
        while (1)
            swiWaitForVBlank();
    }

    // Set material properties
    glMaterialf(GL_AMBIENT, RGB15(8, 8, 8));
    glMaterialf(GL_DIFFUSE, RGB15(24, 24, 24));
    glMaterialf(GL_SPECULAR, RGB15(0, 0, 0));
    glMaterialf(GL_EMISSION, RGB15(0, 0, 0));

    // Set the viewport to fullscreen
    glViewport(0, 0, 255, 191);

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();

    // Setup the projection matrix for regular drawing
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 256.0 / 192.0, 0.1, 10);

    // Use the modelview matrix while drawing
    glMatrixMode(GL_MODELVIEW);

    printf("Pad/B/X: Move\n"
           "SELECT:  Enable/disable BMP\n"
           "\n"
           "START:   Exit to loader\n");

    float x = 0;
    float y = 0;
    float z = 0;

    bool clear_bmp_enable = true;
    glEnable(GL_CLEAR_BMP);

    while (1)
    {
        swiWaitForVBlank();

        consoleSetCursor(NULL, 0, 6);
        printf("Clear BMP: %s\n", clear_bmp_enable ? "On " : "Off");

        // Refresh keys
        scanKeys();
        uint32_t keys = keysHeld();

        // Rotate model using the pad
        if (keys & KEY_UP)
            y += 0.1;
        if (keys & KEY_DOWN)
            y -= 0.1;
        if (keys & KEY_RIGHT)
            x += 0.1;
        if (keys & KEY_LEFT)
            x -= 0.1;
        if (keys & KEY_B)
            z += 0.1;
        if (keys & KEY_X)
            z -= 0.1;

        if (keysDown() & KEY_SELECT)
        {
            clear_bmp_enable = !clear_bmp_enable;
            if (clear_bmp_enable)
                glEnable(GL_CLEAR_BMP);
            else
                glDisable(GL_CLEAR_BMP);
        }

        if (keys & KEY_START)
            break;

        glLoadIdentity();

        // Setup the camera
        gluLookAt(0, 0, 3,
                  0, 0, 0,
                  0, 1, 0);

        glLight(0, RGB15(31, 31, 31), 0, floattov10(-0.97), 0);

        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_FORMAT_LIGHT0 | POLY_MODULATION);

        glBindTexture(0, textureID);

        // Draw regular models

        glPushMatrix();
            glTranslatef(x, y, z);
            glCallList(teapot_bin);
        glPopMatrix(1);

        glFlush(0);
    }

    glDeleteTextures(1, &textureID);

    return 0;
}
