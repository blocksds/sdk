// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <nds.h>

#include "teapot_bin.h"
#include "teapot.h"

int main(int argc, char *argv[])
{
    glInit();
    consoleDemoInit();

    videoSetMode(MODE_0_3D);

    vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankB(VRAM_B_TEXTURE);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ANTIALIAS);

    // Setup the rear plane
    glClearColor(0, 0, 0, 0); // Set BG to black
    // The BG and polygons will have the same ID unless a polygon is highlighted
    glClearPolyID(0);
    glClearDepth(0x7FFF);

    int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(0, textureID);
    if (glTexImage2D(0, 0, GL_RGBA, 256, 256, 0,
                     TEXGEN_TEXCOORD | GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T,
                     teapotBitmap) == 0)
    {
        printf("Failed to load texture\n");
        while (1)
            swiWaitForVBlank();
    }

    glMaterialShinyness();

    // Set material properties
    glMaterialf(GL_AMBIENT, RGB15(8, 8, 8));
    glMaterialf(GL_DIFFUSE, RGB15(24, 24, 24));
    glMaterialf(GL_SPECULAR, RGB15(0, 0, 0));
    glMaterialf(GL_EMISSION, RGB15(0, 0, 0));

    // Set the viewport to fullscreen
    glViewport(0, 0, 255, 191);

    // Setup the projection matrix for regular drawing
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 256.0 / 192.0, 0.1, 20);

    printf("ABXY:    Move texture\n"
           "Pad:     Rotate\n"
           "START:   Exit to loader\n");

    int32_t tx = 0;
    int32_t ty = 0;

    int32_t rx = 0;
    int32_t ry = 0;
    int32_t rz = 0;

    while (1)
    {
        swiWaitForVBlank();

        // Refresh keys
        scanKeys();
        uint32_t keys = keysHeld();

        // Rotate model using the pad
        if (keys & KEY_UP)
            rz += 2;
        if (keys & KEY_DOWN)
            rz -= 2;
        if (keys & KEY_RIGHT)
            ry += 2;
        if (keys & KEY_LEFT)
            ry -= 2;

        if (keys & KEY_A)
            ty += 1 << 6;
        if (keys & KEY_Y)
            ty -= 1 << 6;
        if (keys & KEY_B)
            tx += 1 << 6;
        if (keys & KEY_X)
            tx -= 1 << 6;

        if (keys & KEY_START)
            break;

        // Adjust the texture matrix to translate the texture coordinates
        glMatrixMode(GL_TEXTURE);
        glLoadIdentity();
        glTranslatef32(tx << 12, ty << 12, 0);

        // Use the modelview matrix while drawing
        glMatrixMode(GL_MODELVIEW);

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
            glRotateXi(rx << 6);
            glRotateYi(ry << 6);
            glRotateZi(rz << 6);
            glCallList(teapot_bin);
        glPopMatrix(1);

        glFlush(0);
    }

    glDeleteTextures(1, &textureID);

    return 0;
}
